#include "UbidotsMicroESP8266.h"

#define TOKEN  "BBFF-N1qCtR8Vgr3pRYuZymc05kASHFwuT8"  // Put here your Ubidots TOKEN
#define WIFISSID "EdimaxADSL" // Put here your Wi-Fi SSID
#define PASSWORD "skoop@123" // Put here your Wi-Fi password

Ubidots client(TOKEN);

void setup(){
    Serial.begin(115200);
    client.wifiConnection(WIFISSID, PASSWORD);
    //client.setDebug(true); // Uncomment this line to set DEBUG on

}
void loop(){
    float value1 = random(50);
    float value2 = random(20);
    client.add("temperature", value1);
    client.add("switch", value2);
    client.sendAll(true);
    delay(5000);
}
