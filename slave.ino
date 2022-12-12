#include <esp_now.h>
#include <WiFi.h>

const int CHANNEL = 0; //Canal do PWM.
const int FREQUENCY = 5e3; //Frequency of PWM.
const int RESOLUTION = 8; //Bits resolution of pwm (goes from 0 to 1023).
const int GPIOs[2] = {15,2}; //PWM output gpios.
const int INCREASE = 100;
const int DECREASE = 101;

int duty = 100;

const int TURN_PWM = 0;
const int id = 2;

bool active = false;
bool is_on = false;

void setupPWM(){
  ledcSetup(CHANNEL,FREQUENCY,RESOLUTION);
  for(int i = 0; i < sizeof(GPIOs)/sizeof(int); i++){
    ledcAttachPin(GPIOs[i],CHANNEL); 
  }
  return;
}

void InitESPNow(){
  if(esp_now_init() == ESP_OK){
    Serial.println("Initialização para protocolo NOW bem-sucedida"); 
  }
  else{
    Serial.println("Initialização para protocolo NOW falhou");
    ESP.restart(); //Reinicando;
  }
  return;
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){
  char sentTo[18];
  snprintf(sentTo,sizeof(sentTo),"%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1],
  mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(data[0]);

  if(data[0] == id){
    active = !active;
    Serial.println(active ? "Placa 2 ativada" : "Placa 2 desativada");
  }
  else if(data[0] == TURN_PWM){
    is_on = !is_on;
  }
  else if(data[0] == INCREASE){
    if(active && duty < 250){
      duty += 10;
    }
  }
  else if(data[0] == DECREASE){
    if(active && duty > 100){
      duty -= 100;
    }
  }
  
  return;
}

void setupNow(){
  WiFi.mode(WIFI_STA); //Settando o ESP como estação. 
  Serial.print("Mac Addres da placa: ");
  Serial.println(WiFi.macAddress()); //Printando o MacAddress da placa.

  InitESPNow(); //Inicializando a placa de acordo com o protocolo ESP NOW.

  esp_now_register_recv_cb(OnDataRecv); //Registrando callback (que roda uma vez que o sinal tenha sido recebido). 
  
  return;
}

void setup() {
  Serial.begin(115200);
  setupNow();
  setupPWM();
}

void loop() {
  
  if(is_on && active){
     ledcWrite(CHANNEL,duty); 
  }
  else{
    ledcWrite(CHANNEL,0);
  }
  
}
