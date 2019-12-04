#include <SoftwareSerial.h>
SoftwareSerial SUART(4, 5); //SRX=Dpin-D2; STX=DPin-D1

#include <MQTTClient.h>
#include <ESP8266WiFi.h>
#include "arduino_secrets.h"

///////Enter sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MQTTClient mqttClient;

//------ MQTT broker settings and topics
const char* broker = "mqtt.thingspeak.com"; 
char mqttUserName[] = "Chevindu";       // Can be any name.  
char mqttPass[] = "LR8CPT2B5FO6UHMT";   // Change this your MQTT API Key from Account > MyProfile.  

//-- published settings
char publish_writeAPIKey[] = "QTJ4FCD3X6UXK1HE";// Change to your channel Write API Key.
long publishChannelID = 859244;               
String publishTopic ="channels/" + String( publishChannelID ) + "/publish/"+String(publish_writeAPIKey);        

//-- subscribed settings Virtuino command 1   
long suscribeChannelID_1 = 859872;
String subscribeTopicFor_Command_1="channels/"+String(suscribeChannelID_1)+"/subscribe/fields/field1";

//-- subscribed settings Virtuino command 2   
long suscribeChannelID_2 = 859873;
String subscribeTopicFor_Command_2="channels/"+String(suscribeChannelID_2)+"/subscribe/fields/field1";

const unsigned long postingInterval = 20L * 1000L; // Post data every 20 seconds.
unsigned long lastUploadedTime = 0;

void connect() {
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  //--- create a random MQTT client id
  char clientID[] ="ESP8266_0000000000";  // For random generation of client ID.
  for (int i = 8; i <18 ; i++) clientID[i]=  char(48+random(10));
    
  Serial.println("Connecting to broker...");
  while (!mqttClient.connect(clientID,mqttUserName,mqttPass)) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("MQTT broker nconnected!");
  
  mqttClient.subscribe(subscribeTopicFor_Command_1);
  mqttClient.subscribe(subscribeTopicFor_Command_2);
}

//====== messageReceived
void messageReceived(String &topic, String &payload) {
 Serial.println("incoming: " + topic + " - " + payload);
 
  //-- check for Virtuino Command 1
  if (topic==subscribeTopicFor_Command_1){
        Serial.println("Command 1 = "+payload);
        int v = payload.toInt();
        if (v>0) SUART.print("A");
        else SUART.print("a");
   }

  //-- check for Virtuino Command 1
  if (topic==subscribeTopicFor_Command_2){
        Serial.println("Command 2 = "+payload);
        int v = payload.toInt();
        if (v>0) SUART.print("B");
        else SUART.print("b");
   }  
}

void setup() {
  
  Serial.begin(115200); //enable Serial Monitor
  SUART.begin(9600); //enable SUART Port
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
   WiFi.begin(ssid, pass);
  mqttClient.begin(broker, wifiClient);
  mqttClient.onMessage(messageReceived);
  connect();

}

void loop() {

  mqttClient.loop();
  delay(10);  // <- fixes some issues with WiFi stability
  if (!mqttClient.connected()){
    connect();
    return;
  }
 
  if (millis() - lastUploadedTime > postingInterval) { // The uploading interval must be > 15 seconds 
    int sensorValue_1 = random(100); // replace with your sensor value
    int sensorValue_2=random(100);  // replace with your sensor value
    //int sensorValue_3=random(100);  // if you want to use three sensors enable this line
    
    String dataText = String("field1=" + String(sensorValue_1)+ "&field2=" + String(sensorValue_2));
    //String dataText = String("field1=" + String(sensorValue_1)+ "&field2=" + String(sensorValue_2)+"&field3=" + String(sensorValue_3)); // example for publish tree sensors
    boolean result = mqttClient.publish(publishTopic, dataText);
    
    if (result) Serial.println("Data has been published succesfully"); 
    else Serial.println("Unable to publish data");
    
    lastUploadedTime = millis();
  }
}
