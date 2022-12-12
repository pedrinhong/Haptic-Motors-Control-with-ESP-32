#include <DabbleESP32.h>

//GPIO output.
const int gpio = 15;

//Descrição do PWM.
const int channel = 0; //ESP 32 channel.
const int resolution = 8; //PWM bits resolution (goes from 0 up to 255).
int frequency = 0; //PWM frequency.
int cycle = 125;

void setup() {
  //Setting serial communitation configurations.
  Serial.begin(115200); //Baud rate.
  Dabble.begin("Pedro's_ESP32");
  ledcSetup(channel,frequency,resolution);
  ledcAttachPin(gpio,channel);
}

void loop() {
  Dabble.processInput();
  if(GamePad.isUpPressed()){
    Serial.print("Aumentou\n");
    frequency += 100; 
    ledcSetup(channel,frequency,resolution);
    delay(200);
  }
  else if(GamePad.isDownPressed()){
    Serial.print("Diminuiu\n");
    frequency -= 100;
    ledcSetup(channel,frequency,resolution);
    delay(200);
  }
  else if(GamePad.isSquarePressed()){
    Serial.print("Play\n");
    ledcWrite(channel,cycle);
  }
  else{
    ledcWrite(channel,0); 
  }
}
