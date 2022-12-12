
//Pacotes a serem incluídos.

#include <DabbleESP32.h>
#include <esp_now.h> //Pacote para implementação do protocolo ESP_NOW.
#include <WiFi.h> //Pacote para pairing das placas.

//Constantes referentes ao PWM dos motores hápticos.
const int CHANNEL = 0; //Canal do PWM.
const int FREQUENCY = 5e3; //Frequency of PWM.
const int RESOLUTION = 8; //Bits resolution of pwm (goes from 0 to 255).
const int GPIOs[2] = {15,2}; //PWM output gpios.
uint8_t INCREASE = 100;
uint8_t DECREASE = 101;

//Constantes referentes à comunicação via protocolo ESP NOW.
//If we use {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, we broadcast the signal to every available board.
//If we use each MacAddress, we can determine which board we wanna transmite the signal.
const int nSlaves = 1; //Quantos placas slaves há.
uint8_t TURN_PWM = 0; //Comando para ligar/desligar PWM.

//Endereço das placas "slaves" para pairing via ESP NOW.
uint8_t macSlaves[nSlaves][6] = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}}; // MacAddress of each "slave" board.

//Variáveis referentes ao estado do PWM (ativação e intensidade).
bool active = false;
bool is_on = false;
int duty = 100; // Ciclo de carga. Inicialmente, um pouco abaixo de 50%.

void setupDabble(){
  Dabble.begin("Pedro's ESP32");
  return;
}

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

void send(uint8_t dados){
    uint8_t broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    for(int i = 0; i < nSlaves; i++){
      esp_err_t result; //Instanciando objeto para armazenar resultado do envio de dados.
      result = esp_now_send(broadcast, (uint8_t *) &dados, sizeof(dados));
      Serial.println(result == ESP_OK ? "Envio bem sucedido." : "Envio falhou.");
    }
    return;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
  char sentTo[18];
  snprintf(sentTo,sizeof(sentTo),"%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1],
  mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.print("Enviado para: ");
  Serial.println(sentTo);

  Serial.print("Status de envio: ");
  Serial.print(status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");

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
  return;
}

void setup() {
  Serial.begin(115200); //Settando baud-rate do terminal serial.
  setupNow();
  setupDabble(); 
  setupPWM();
}

void activeBoard(int id){
  send(id);
}

void turnPWM(){
  send(TURN_PWM);
}

void loop() {
  Dabble.processInput();

  // Ativando/Desativando as placas.
  if(GamePad.isCirclePressed()){
    active = !active; //Alterando estado.
    Serial.println(active ? "Placa 1 ativada" : "Placa 1 desativada");
    delay(200);
  }
  else if(GamePad.isSquarePressed()){
     activeBoard(2); //Ativando placa 2.
     delay(200);
  }
  else if(GamePad.isTrianglePressed()){
    activeBoard(3); //Ativando placa 3.
    delay(200);  
  }

  // Aumentando/diminuindo intensidade do PWM.
  if(GamePad.isUpPressed() && active){
    if(duty < 250){
      duty += 10;
    }
    Serial.print("Ciclo de carga de ");
    Serial.println(duty);
    send(INCREASE);
    delay(200);
  }
  else if(GamePad.isDownPressed() && active){
    if(duty > 100){
      duty -= 10;
    }
    Serial.print("Ciclo de carga de ");
    Serial.println(duty);
    send(DECREASE);
    delay(200);
  }
  else if(GamePad.isStartPressed()){
    is_on = !is_on;
    turnPWM();
    Serial.println(is_on ? "Motores ligados" : "Motores desligados");
    delay(200);
  }

  if(is_on && active){
     ledcWrite(CHANNEL,duty); 
  }
  else{
    ledcWrite(CHANNEL,0);
  }
  
}
