/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsESPMQTT.h"

/****************************************
 * Define Constants
 ****************************************/
#define TOKEN "BBFF-3de67900e81e3abfbb1ca222366803153a4" // Your Ubidots TOKEN
#define WIFINAME "EdimaxADSL" //Your SSID
#define WIFIPASS "skoop@123" // Your Wifi Pass
//#define MQTTCLIENT "CTVid"  //MQTT Client Name

Ubidots client(TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup() {
  // put your setup code here, to run once:
  client.ubidotsSetBroker("business.api.ubidots.com");
  client.setDebug(true); // Pass a true or false bool value to activate debug messages
  Serial.begin(115200);
  client.wifiConnection(WIFINAME, WIFIPASS);
  client.begin(callback);
  }

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
      client.reconnect();
      }
  
  // Publish values to 2 different data sources
  
  client.add("stuff", random(50)); //Insert your variable Labels and the value to be sent
  client.ubidotsPublish("source1");
  client.add("stuff", random(70));
  client.add("more-stuff", random(100));
  client.ubidotsPublish("source2");
  client.loop();
  }
