var index = 0;

/* Set up all the different states 
    index 1 : Summary of Project, Items
    index 2 : Power Rails
    index 3 : Potentiometer
    index 4 : Servo
    index 5 : Code
*/

var states = [
  { //1 : Summary of Project, Items
    boardLeft: "https://rawcdn.githack.com/sycrus/thesis-studio-2/969fd1aa1fb30e05e81c59cc45532485da5251c7/assets/images/overlay/a_l_pot.png",
    boardRight: "https://rawcdn.githack.com/sycrus/thesis-studio-2/6aec96e3f0df480c659672d2c401904ce315ed49/assets/images/overlays/a_r_servo.png",
    side: "https://rawcdn.githack.com/sycrus/thesis-studio-2/6aec96e3f0df480c659672d2c401904ce315ed49/assets/images/diagrams/finished.png",
    sideWidth:"2.625",
    sideHeight:"1.75",
    instruction: "You will need:      5K Potentiometer, Micro Servo",
    instructionAudio:"#parts",
  },
  {//2 : Top Power Rail
    boardLeft: "https://rawcdn.githack.com/sycrus/thesis-studio-2/969fd1aa1fb30e05e81c59cc45532485da5251c7/assets/images/overlay/a_l_power.png",
    boardRight: "https://rawcdn.githack.com/sycrus/thesis-studio-2/600032a246af2b25f6442f977faf6a04ef9d4461/assets/images/overlays/a_r_power.png",
    side: "https://rawcdn.githack.com/sycrus/thesis-studio-2/00c03b4033ded5700b65ebe487661e9e50f987c7/assets/images/diagrams/power1.png",
    sideWidth:"1",
    sideHeight:"2",
    instruction: "Connect the ESP32s 3.3V pin and right ground pin to the top rail",
    instructionAudio:"#power1",
  },
   {//3 : Bottom Power Rail
    boardLeft: "https://rawcdn.githack.com/sycrus/thesis-studio-2/969fd1aa1fb30e05e81c59cc45532485da5251c7/assets/images/overlay/a_l_power.png",
    boardRight: "https://rawcdn.githack.com/sycrus/thesis-studio-2/600032a246af2b25f6442f977faf6a04ef9d4461/assets/images/overlays/a_r_power.png",
    side: "https://rawcdn.githack.com/sycrus/thesis-studio-2/00c03b4033ded5700b65ebe487661e9e50f987c7/assets/images/diagrams/power2.png",
    sideWidth:"2.2",
    sideHeight:"2",
    instruction: "Connect the ESP32s 5V pin and left ground pin to the bottom rails",
    instructionAudio:"#power2",
  },
  {//4 : Potentiometer

    boardLeft: "https://rawcdn.githack.com/sycrus/thesis-studio-2/969fd1aa1fb30e05e81c59cc45532485da5251c7/assets/images/overlay/a_l_pot.png",
    boardRight: "https://rawcdn.githack.com/sycrus/thesis-studio-2/600032a246af2b25f6442f977faf6a04ef9d4461/assets/images/overlays/a_r_servo_greyed.png",
    side: "https://rawcdn.githack.com/sycrus/thesis-studio-2/6aec96e3f0df480c659672d2c401904ce315ed49/assets/images/diagrams/pot2.png",
    sideWidth:"2.3",
    sideHeight:"2",
    instruction: "Connect the potentiometer to the ESP32",
    instructionAudio:"#potentiometer",
  },
  {//5 : Servo
    boardLeft: "https://rawcdn.githack.com/sycrus/thesis-studio-2/600032a246af2b25f6442f977faf6a04ef9d4461/assets/images/overlays/a_l_pot_greyed.png",
    boardRight: "https://rawcdn.githack.com/sycrus/thesis-studio-2/600032a246af2b25f6442f977faf6a04ef9d4461/assets/images/overlays/a_r_servo.png",
    side: "https://rawcdn.githack.com/sycrus/thesis-studio-2/6aec96e3f0df480c659672d2c401904ce315ed49/assets/images/diagrams/servo2.png",
    sideWidth:"2.08",
    sideHeight:"2",
    instruction: "Connect the servo to the ESP32",
    instructionAudio:"#servo",
  }
];

AFRAME.registerSystem('markerhandler', {
    init: function () {
        console.log("in markerhandler system init");
        window.addEventListener('loaded', this.onSceneLoaded.bind(this));
    },
    onSceneLoaded: function(evt) {
        console.log("in voice-command system onSceneLoaded listener");
    },
});

AFRAME.registerComponent('markerhandler', {
  
    init: function() {
        console.log("index:" + index);
        const marker = document.querySelector("#instructionMarker");
        const nextButton = document.querySelector("#nextButton");
        const prevButton = document.querySelector("#prevButton");
      
        const boardLeftImage = document.querySelector("#boardLeft");
        const boardRightImage = document.querySelector("#boardRight");
        const sideImage = document.querySelector("#side");
        const instructionText = document.querySelector("#instructionText");
        const instructionPanel = document.querySelector("#instructionPanel");
        const stepNumber = document.querySelector("#stepNumber");
        const potModel = document.querySelector("#potModel");
        const potButton1 = document.querySelector("#pb1_button"); const potButton2 = document.querySelector("#pb2_button"); const potButton3 = document.querySelector("#pb3_button");
        const servoModel = document.querySelector("#servoModel");
        const servoButton1 = document.querySelector("#sb1_button"); const servoButton2 = document.querySelector("#sb2_button"); const servoButton3 = document.querySelector("#sb3_button");

        boardLeftImage.setAttribute("src", states[index].boardLeft);
        boardRightImage.setAttribute("src", states[index].boardRight);
        sideImage.setAttribute("src", states[index].side);
        sideImage.setAttribute("width", states[index].sideWidth);
        sideImage.setAttribute("height", states[index].sideHeight);
        instructionText.setAttribute("value", states[index].instruction);
        instructionPanel.setAttribute("sound", "src", states[index].instructionAudio);
        stepNumber.setAttribute("value", index+1);
        
        nextButton.addEventListener("click", function (e){
           if (index < states.length - 1) {
              index++;
            } else {
              index = states.length - 1;
            }
          
          if (index == 3) {
            console.log("pot");
            potModel.setAttribute("visible", "true");
            servoModel.setAttribute("visible", "false");
            potButton1.setAttribute("class", "clickable"); potButton2.setAttribute("class", "clickable"); potButton3.setAttribute("class", "clickable");
            servoButton1.setAttribute("class", "not-clickable"); servoButton2.setAttribute("class", "not-clickable"); servoButton3.setAttribute("class", "not-clickable");
            
          }
          else if (index == 4) {
            console.log("servo");
            potModel.setAttribute("visible", "false");
            servoModel.setAttribute("visible", "true");
            potButton1.setAttribute("class", "not-clickable"); potButton2.setAttribute("class", "not-clickable"); potButton3.setAttribute("class", "not-clickable");
            servoButton1.setAttribute("class", "clickable"); servoButton2.setAttribute("class", "clickable"); servoButton3.setAttribute("class", "clickable");
          }
          else {
            potModel.setAttribute("visible", "false");
            servoModel.setAttribute("visible", "false");
            potButton1.setAttribute("class", "not-clickable"); potButton2.setAttribute("class", "not-clickable"); potButton3.setAttribute("class", "not-clickable");
            servoButton1.setAttribute("class", "not-clickable"); servoButton2.setAttribute("class", "not-clickable"); servoButton3.setAttribute("class", "not-clickable");
          }
            boardLeftImage.setAttribute("src", states[index].boardLeft);
            boardRightImage.setAttribute("src", states[index].boardRight);
            sideImage.setAttribute("src", states[index].side);
            sideImage.setAttribute("width", states[index].sideWidth);
            sideImage.setAttribute("height", states[index].sideHeight);
            instructionText.setAttribute("value", states[index].instruction);
            instructionPanel.setAttribute("sound", "src", states[index].instructionAudio);
            stepNumber.setAttribute("value", index+1);
        });
      
        prevButton.addEventListener("click", function (e){         
           console.log("PREVIOUS");
           if (index > 0) {
              index--;
            } else {
              index = 0;
            }
            if (index == 3) {
              console.log("pot");
              potModel.setAttribute("visible", "true");
              servoModel.setAttribute("visible", "false");
              potButton1.setAttribute("class", "clickable"); potButton2.setAttribute("class", "clickable"); potButton3.setAttribute("class", "clickable");
              servoButton1.setAttribute("class", "not-clickable"); servoButton2.setAttribute("class", "not-clickable"); servoButton3.setAttribute("class", "not-clickable");

            }
            else if (index == 4) {
              console.log("servo");
              potModel.setAttribute("visible", "false");
              servoModel.setAttribute("visible", "true");
              potButton1.setAttribute("class", "not-clickable"); potButton2.setAttribute("class", "not-clickable"); potButton3.setAttribute("class", "not-clickable");
              servoButton1.setAttribute("class", "clickable"); servoButton2.setAttribute("class", "clickable"); servoButton3.setAttribute("class", "clickable");
            }
            else {
              potModel.setAttribute("visible", "false");
              servoModel.setAttribute("visible", "false");
              potButton1.setAttribute("class", "not-clickable"); potButton2.setAttribute("class", "not-clickable"); potButton3.setAttribute("class", "not-clickable");
              servoButton1.setAttribute("class", "not-clickable"); servoButton2.setAttribute("class", "not-clickable"); servoButton3.setAttribute("class", "not-clickable");
            }
            boardLeftImage.setAttribute("src", states[index].boardLeft);
            boardRightImage.setAttribute("src", states[index].boardRight);
            sideImage.setAttribute("src", states[index].side);
            sideImage.setAttribute("width", states[index].sideWidth);
            sideImage.setAttribute("height", states[index].sideHeight);
            instructionText.setAttribute("value", states[index].instruction);
            instructionPanel.setAttribute("sound", "src", states[index].instructionAudio);
            stepNumber.setAttribute("value", index+1);
        });
      },
            
});
