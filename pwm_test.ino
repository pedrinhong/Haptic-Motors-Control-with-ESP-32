//GPIO output.
const int gpios = 15;

//Descrição do PWM.
const int frequency = 5e3; //PWM frequency.
const int channel = 0; //ESP 32 channel.
const int resolution = 8; //PWM bits resolution (goes from 0 up to 255).

void setup() {
  Serial.begin(115200);
  ledcSetup(channel,frequency,resolution);
  ledcAttachPin(gpios,channel);
  ledcSetup(1,frequency,resolution);
  ledcAttachPin(2,1);
  //for(int i = 0; i < 2; i++){
   // ledcSetup(channels[i],frequency,resolution);
    //ledcAttachPin(gpios[i],channels[i]);  
  //}
}

void loop() {
  for(int i = 0; i < 255; i += 10){
    Serial.print("Aumentou para: ");
    Serial.println(i);
    //for(int j = 0; j < 2; j++){
     // ledcWrite(channels[j],i); 
    //}
    ledcWrite(channel,i);
    ledcWrite(1,i);
    delay(2000);
  }
}
