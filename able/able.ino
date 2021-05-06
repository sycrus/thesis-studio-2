/*********
  Based on ESP32-mpu-6050-web-server example by Rui Santos: 
  https://RandomNerdTutorials.com/esp32-mpu-6050-web-server/
*********/

// TODO: Configure your WiFi here
#define WIFI_SSID "<your_SSID"
#define WIFI_PSK  "your_password"

// able. START ====================================================================================
#define PWM_PIN 18
#define POT_PIN 35
#define READ_PIN
int potValue = 0 ;
int pwmValue = 0 ;

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include <cert.h>
#include <private_key.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <util.hpp>

using namespace httpsserver;

//able. function declarations
void initWiFi();
void initSPIFFS();
SSLCert * getCertificate();
void handleSPIFFS(HTTPRequest * req, HTTPResponse * res);
void handleGetButtonState(HTTPRequest * req, HTTPResponse * res);

HTTPSServer * secureServer;
JSONVar readings;
// able. END ==========================================================================================

// Learner-edited START -------------------------------------------------------------------------------

#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo

int ADC_Max = 4096;     // This is the default ADC max value on the ESP32 (12 bit ADC width);
                        // this width can be set (in low-level oode) from 9-12 bits, for a
                        // a range of max values of 512-4096

int val;    // variable to read the value from the analog pin

// setting PWM properties, ledcWrite
const int freq = 10;
const int ledChannel = 0;
const int resolution = 8;

//Learner END ------------------------------------------------------------------------------------------

void setup() {

  //ARMS code -- All inputs as we are reading to display
  pinMode(POT_PIN, INPUT);
  pinMode(PWM_PIN, OUTPUT);

  Serial.begin(115200);
  initWiFi();
  initSPIFFS();

  // Allow allocation of all timers 
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);// Standard 50hz servo
  myservo.attach(PWM_PIN, 500, 2400);   // attaches the servo on pin 18 to the servo object
  // using SG90 servo min/max of 500us and 2400us
  // for MG995 large servo, use 1000us and 2000us,
  // which are the defaults, so this line could be
  // "myservo.attach(servoPin);"
}

void loop() {
  server();   // This loop is reserved for server code
  myLoop();   // This loop runs the learner's code
}

void server() {
  static unsigned long lastTime = 0;
  const long interval = 10;
  static bool state = 0;

  unsigned long now = millis();

  if ( now - lastTime > interval && state == 0) {
    state = 1;
    lastTime = now;
    secureServer->loop();
  }
  if ( now - lastTime > interval && state == 1) {
    state = 0;
    lastTime = now;
  }
}

void myLoop() {
  static unsigned long lastTime = 0;
  const long interval = 15; // <------------------  delays go here
  static bool state = 0;

  unsigned long now = millis();

  if ( now - lastTime > interval && state == 0) {
    state = 1;
    lastTime = now;
    //my code goes here --> standard loop function
    
    val = analogRead(POT_PIN);            // read the value of the potentiometer (value between 0 and 1023)
    val = map(val, 0, ADC_Max, 0, 180);     // scale it to use it with the servo (value between 0 and 180)

    myservo.write(val);                  // set the servo position according to the scaled value
  }
  if ( now - lastTime > interval && state == 1) {
    state = 0;
    lastTime = now;
  }
}
