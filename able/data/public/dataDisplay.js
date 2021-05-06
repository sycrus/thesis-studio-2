
  const scale = (num, in_min, in_max, out_min, out_max) => { return (num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

  var pin35=0; var pin18=0;

  function updatePins() {
    $.ajax({
      // Target
      url: '/api/events',
      // Method
      type: 'GET',
      // Data we expect back
      dataType: 'json',
    }).done(res => {

      //get pin data
      pin35 = res[0].GPIO35; //Potentiometer Analog Values
      pin18 = res[0].GPIO18; //Servo PWM

      //Marker
      $analogValueText = $('#analogValueText')[0];
      $analogDial1= $('#analogDial')[0];
      $analogDialBack1 = $('#analogDialBack')[0];
      $analogValueText.setAttribute('value', pin35);
      $analogDial1.setAttribute('geometry','thetaLength', scale(pin35, 0, 4095, 0, 270));
      $analogDialBack1.setAttribute('geometry','thetaLength', 270 - scale(pin35, 0, 4095, 0, 270));

    }).always(() => {
      // Synchronize again after 1s
      setTimeout(updatePins, 100);
    });
  }
  // When the page is ready...
  $(document).ready(() => {
    updatePins();
    // Data
    var line1 = new TimeSeries();
    setInterval(function() {
      line1.append(new Date().getTime(), pin18);
      // line1.append(new Date().getTime(), pin18);
    }, 100);
    // setInterval(function() { line1.append(new Date().getTime(), pin18); }, 100);

    var smoothie = new SmoothieChart({
      interpolation: "step", maxValue: 1.2, minValue: -0.2, maxValueScale: 1, minValueScale: 1,
      grid: { sharpLines: true, strokeStyle: "#f0739d", fillStyle: "#2b2e4a", lineWidth: 0.5, millisPerLine: 200, verticalSections: 5},
      labels: { disabled: false, fontFamily: "sans-serif", fillStyle: "#f0739d", fontSize: "20", showIntermediateLabels: true, intermediateLabelSameAxis: true}
    });
    smoothie.addTimeSeries(line1, { strokeStyle: "#f0739d", lineWidth: 2 });
    smoothie.streamTo(document.getElementById("my-canvas"), 100 /*delay*/);
  });
