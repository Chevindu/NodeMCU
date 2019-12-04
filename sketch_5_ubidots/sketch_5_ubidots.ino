
/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsESP8266.h"
#include <SoftwareSerial.h>
SoftwareSerial SUART(4, 5); //SRX=Dpin-D2; STX=DPin-D1

/****************************************
 * Define Constants
 ****************************************/
namespace { 
  const char * WIFISSID = "EdimaxADSL"; // Assign your WiFi SSID 
  const char * PASSWORD = "skoop@123"; // Assign your WiFi password
  const char * TOKEN = "BBFF-3de67900e81e3abfbb1ca222366803153a4"; // Assign your Ubidots TOKEN
}

Ubidots client(TOKEN);

/****************************************
 * Main Functions
 ****************************************/ 
void setup() {
  Serial.begin(115200);
  client.wifiConnection(WIFISSID, PASSWORD);
  SUART.begin(9600); //enable SUART Port
}

void loop() {
  client.readData(); // Reads the command from the logger
  delay(1000);
}
