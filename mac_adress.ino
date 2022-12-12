
#include <WiFi.h> 
#include <esp_now.h>

//ESP1: {0xE0,0xE2,0xE6,0x0C,0x69,0x0C}
//ESP2: [0xE0,0xE2,0xE6,0x0C,0x6B,0x0C]

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}

void loop() {
  // put your main code here, to run repeatedly:

}
