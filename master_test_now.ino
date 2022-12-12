
//Pacotes a serem incluídos.

#include <esp_now.h> //Pacote para implementação do protocolo ESP_NOW.
#include <WiFi.h> //Pacote para pairing das placas.

//Constantes referentes ao PWM dos motores hápticos.

const int CHANNEL = 0; //Canal do PWM.
const int FREQUENCY = 5e3; //Frequency of PWM.
const int RESOLUTION = 10; //Bits resolution of pwm (goes from 0 to 1023).
const int GPIOs[4] = {15,2,4,5}; //PWM output gpios.

//Constantes referentes à comunicação via protocolo ESP NOW.

//If we use {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, we broadcast the signal to every available board.
//If we use each MacAddress, we can determine which board we wanna transmite the signal.
const int nSlaves = 1; //Quantos placas slaves há.
uint8_t macSlaves[nSlaves][6] = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}}; // MacAddress of each "slave" board.

//void setupPWM(){
//}

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

void send(){
    uint8_t dados[2] = {random(0,255),random(0,255)};
    uint8_t broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    for(int i = 0; i < nSlaves; i++){
      esp_err_t result; //Instanciando objeto para armazenar resultado do envio de dados.
      /*char sentTo[18];
      snprintf(sentTo,sizeof(sentTo),"%02x:%02x:%02x:%02x:%02x:%02x", macSlaves[0][0], macSlaves[0][1],
      macSlaves[0][2], macSlaves[0][3], macSlaves[0][4], macSlaves[0][5]);
      Serial.print("Enviar para: ");
      Serial.println(sentTo);*/
      result = esp_now_send(broadcast, (uint8_t *) &dados, sizeof(dados));
      Serial.println(result == ESP_OK ? "Envio bem sucedido." : "Envio falhou.");
    }
    delay(2000);
    return;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
  char sentTo[18];
  snprintf(sentTo,sizeof(sentTo),"%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1],
  mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.print("Enviado para: ");
  Serial.println(sentTo);

  Serial.print("Satus de envio");
  Serial.print(status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");

  send();
  return;
}

void setupNow(){
  WiFi.mode(WIFI_STA); //Settando o ESP como estação. 
  Serial.print("Mac Addres da placa: ");
  Serial.println(WiFi.macAddress()); //Printando o MacAddress da placa.

  InitESPNow(); //Inicializando a placa de acordo com o protocolo ESP NOW.

  esp_now_peer_info_t slave; //Instanciando um objeto para armazenar as informações de pareamento.
  
  for(int i = 0; i < nSlaves; i++){
    slave.channel = CHANNEL;
    slave.encrypt = 0;
    memset(&slave, 0, sizeof(slave));
    memcpy(slave.peer_addr, macSlaves[i],sizeof(macSlaves[i]));
    esp_now_add_peer(&slave); //Pairing placas.
  }  

  esp_now_register_send_cb(OnDataSent); //Registrando callback (que roda uma vez que o sinal tenha sido enviado.

  send(); //Função que implementa o envio do sinal. 

  return;
}

void setup() {
  Serial.begin(115200); //Settando baud-rate do terminal serial.
  setupNow();
  //setupPWM(); 
}

//Não há nada no loop por causa da presença de uma função callback.
void loop() {
}
