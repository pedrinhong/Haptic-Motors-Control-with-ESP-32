#include <esp_now.h>
#include <WiFi.h>


void setupPWM(){
  
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

  Serial.print("Recebido: ");
  Serial.print(data[0]);
  Serial.print(" ");
  Serial.print(data[1]);
  Serial.print("--> de: ");
  Serial.println(sentTo);
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
}
