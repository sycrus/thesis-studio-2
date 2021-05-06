//Edited HTTPS code from REST-API example

// We need to specify some content-type mapping, so the resources get delivered with the
// right content type and are displayed correctly in the browser
char contentTypes[][2][32] = {
  {".html", "text/html"},
  {".css",  "text/css"},
  {".js",   "application/javascript"},
  {".json", "application/json"},
  {".png",  "image/png"},
  {".jpg",  "image/jpg"},
  {"", ""}
};

// Define the name of the directory for public files in the SPIFFS parition
#define DIR_PUBLIC "/public"

/**
   This handler function will get pin values, and store them as a JSON object.

   Currently, all demo inputs and outputs are hard-coded.
   
   This JSON object will be retrieved by the website to update AR values.
*/
void handleGetButtonState(HTTPRequest * req, HTTPResponse * res) {

  DynamicJsonBuffer jsonBuffer(2); //2 inputs
  JsonArray& arr = jsonBuffer.createArray();

  JsonObject& eventObj = arr.createNestedObject();
  eventObj["GPIO35"] = analogRead(POT_PIN);
  eventObj["GPIO18"] = digitalRead(PWM_PIN);

  res->setHeader("Content-Type", "application/json");
  arr.printTo(*res);
}

/**
  Checks SPIFFS
  Generates security cert
  Creates and starts secure server
*/
void initSPIFFS() {
  if (!SPIFFS.begin(false)) {
    // If SPIFFS does not work, we wait for serial connection...
    while (!Serial);
    delay(1000);

    // Ask to format SPIFFS using serial interface
    Serial.print("Mounting SPIFFS failed. Try formatting? (y/n): ");
    while (!Serial.available());
    Serial.println();

    // If the user did not accept to try formatting SPIFFS or formatting failed:
    if (Serial.read() != 'y' || !SPIFFS.begin(true)) {
      Serial.println("SPIFFS not available. Stop.");
      while (true);
    }
    Serial.println("SPIFFS has been formated.");
  }
  Serial.println("SPIFFS has been mounted.");

  //From Self Signed Certificate ino:
  // First, we create an empty certificate:
  Serial.println("Creating a new self-signed certificate.");
  Serial.println("This may take up to a minute, so be patient ;-)");

  SSLCert *cert = new SSLCert();
  int createCertResult = createSelfSignedCert(
                           *cert,
                           KEYSIZE_2048,
                           "CN=myesp32.local,O=FancyCompany,C=DE",
                           "20190101000000",
                           "20300101000000"
                         );

  // Now check if creating that worked
  if (createCertResult != 0) {
    Serial.printf("Cerating certificate failed. Error Code = 0x%02X, check SSLCert.hpp for details", createCertResult);
    while (true) delay(500);
  }
  Serial.println("Creating the certificate was successful");

  //SECURE SERVER
  // Create the server with the certificate we loaded before
  secureServer = new HTTPSServer(cert);

  //   We register the SPIFFS handler as the default node, so every request that does
  //   not hit any other node will be redirected to the file system.
  ResourceNode * spiffsNode = new ResourceNode("", "", &handleSPIFFS);
  secureServer->setDefaultNode(spiffsNode);

  // Add a handler that serves the current pin status at GET /api/pin25
  ResourceNode * inputNode = new ResourceNode("/api/events", "GET", &handleGetButtonState);
  secureServer->registerNode(inputNode);

  // Add a handler that serves the current system uptime at GET /api/uptime
  //  ResourceNode * uptimeNode = new ResourceNode("/api/uptime", "GET", &handleGetUptime);
  //  secureServer->registerNode(uptimeNode);

  Serial.println("Starting server...");
  secureServer->start();
  if (secureServer->isRunning()) {
    Serial.println("Server ready.");
  }
}


/**
   This function will either read the certificate and private key from SPIFFS or
   create a self-signed certificate and write it to SPIFFS for next boot
*/
SSLCert * getCertificate() {
  // Try to open key and cert file to see if they exist
  File keyFile = SPIFFS.open("/key.der");
  File certFile = SPIFFS.open("/cert.der");

  // If not, create them
  if (!keyFile || !certFile || keyFile.size() == 0 || certFile.size() == 0) {
    Serial.println("No certificate found in SPIFFS, generating a new one for you.");
    Serial.println("If you face a Guru Meditation, give the script another try (or two...).");
    Serial.println("This may take up to a minute, so please stand by :)");

    SSLCert * newCert = new SSLCert();
    // The part after the CN= is the domain that this certificate will match, in this
    // case, it's esp32.local.
    // However, as the certificate is self-signed, your browser won't trust the server
    // anyway.
    int res = createSelfSignedCert(
                *newCert,
                KEYSIZE_2048,
                "CN=myesp32.local,O=FancyCompany,C=DE",
                "20190101000000",
                "20300101000000"
              );
    if (res == 0) {
      // We now have a certificate. We store it on the SPIFFS to restore it on next boot.

      bool failure = false;
      // Private key
      keyFile = SPIFFS.open("/key.der", FILE_WRITE);
      if (!keyFile || !keyFile.write(newCert->getPKData(), newCert->getPKLength())) {
        Serial.println("Could not write /key.der");
        failure = true;
      }
      if (keyFile) keyFile.close();

      // Certificate
      certFile = SPIFFS.open("/cert.der", FILE_WRITE);
      if (!certFile || !certFile.write(newCert->getCertData(), newCert->getCertLength())) {
        Serial.println("Could not write /cert.der");
        failure = true;
      }
      if (certFile) certFile.close();

      if (failure) {
        Serial.println("Certificate could not be stored permanently, generating new certificate on reboot...");
      }

      return newCert;

    } else {
      // Certificate generation failed. Inform the user.
      Serial.println("An error occured during certificate generation.");
      Serial.print("Error code is 0x");
      Serial.println(res, HEX);
      Serial.println("You may have a look at SSLCert.h to find the reason for this error.");
      return NULL;
    }

  } else {
    Serial.println("Reading certificate from SPIFFS.");

    // The files exist, so we can create a certificate based on them
    size_t keySize = keyFile.size();
    size_t certSize = certFile.size();

    uint8_t * keyBuffer = new uint8_t[keySize];
    if (keyBuffer == NULL) {
      Serial.println("Not enough memory to load private key");
      return NULL;
    }
    uint8_t * certBuffer = new uint8_t[certSize];
    if (certBuffer == NULL) {
      delete[] keyBuffer;
      Serial.println("Not enough memory to load certificate");
      return NULL;
    }
    keyFile.read(keyBuffer, keySize);
    certFile.read(certBuffer, certSize);

    // Close the files
    keyFile.close();
    certFile.close();
    Serial.printf("Read %u bytes of certificate and %u bytes of key from SPIFFS\n", certSize, keySize);
    return new SSLCert(certBuffer, certSize, keyBuffer, keySize);
  }
}

/**
   This handler function will try to load the requested resource from SPIFFS's /public folder.

   If the method is not GET, it will throw 405, if the file is not found, it will throw 404.
*/
void handleSPIFFS(HTTPRequest * req, HTTPResponse * res) {

  // We only handle GET here
  if (req->getMethod() == "GET") {
    // Redirect / to /index.html
    std::string reqFile = req->getRequestString() == "/" ? "/index.html" : req->getRequestString();

    // Try to open the file
    std::string filename = std::string(DIR_PUBLIC) + reqFile;

    // Check if the file exists
    if (!SPIFFS.exists(filename.c_str())) {
      // Send "404 Not Found" as response, as the file doesn't seem to exist
      res->setStatusCode(404);
      res->setStatusText("Not found");
      res->println("404 Not Found");
      return;
    }

    File file = SPIFFS.open(filename.c_str());

    // Set length
    res->setHeader("Content-Length", httpsserver::intToString(file.size()));

    // Content-Type is guessed using the definition of the contentTypes-table defined above
    int cTypeIdx = 0;
    do {
      if (reqFile.rfind(contentTypes[cTypeIdx][0]) != std::string::npos) {
        res->setHeader("Content-Type", contentTypes[cTypeIdx][1]);
        break;
      }
      cTypeIdx += 1;
    } while (strlen(contentTypes[cTypeIdx][0]) > 0);

    // Read the file and write it to the response
    uint8_t buffer[256];
    size_t length = 0;
    do {
      length = file.read(buffer, 256);
      res->write(buffer, length);
    } while (length > 0);

    file.close();
  } else {
    // If there's any body, discard it
    req->discardRequestBody();
    // Send "405 Method not allowed" as response
    res->setStatusCode(405);
    res->setStatusText("Method not allowed");
    res->println("405 Method not allowed");
  }
}
