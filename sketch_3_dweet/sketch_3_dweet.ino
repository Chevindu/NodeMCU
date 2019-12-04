#include "arduino_secrets.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
SoftwareSerial SUART(4, 5); //SRX=Dpin-D2; STX=DPin-D1

// WiFi parameters
char ssid[] = SECRET_SSID; //WiFi name
char pass[] = SECRET_PASS; //WiFi password
String thingName="CTVid";  //For grouping all the data together
const char* host = "dweet.io"; //Host for tcp connection

//Initialize the names & values of our metrics for dweet.io
String arrayVariableNames[]={"x_value", "y_value"};
int arrayVariableValues[]={0,0};

//tells the number of arguments inside each array
int numberVariables=sizeof(arrayVariableValues)/sizeof(arrayVariableValues[0]);

void connect() {
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // when failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();
}

/*
 * Get the string URL to post dweet
 */
String getDweetString(){
  int i = 0;
  String dweetHttpGet="GET /dweet/for/";  //use the dweet GET to post to dweet
  dweetHttpGet=dweetHttpGet+String(thingName)+"?";

  for(i=0;i<(numberVariables);i++){
    if(i==numberVariables-1){
      //the last one doesnt have an "&" at the end
      dweetHttpGet=dweetHttpGet + String(arrayVariableNames[i]) + "=" + String(arrayVariableValues[i]);
    }
    else
      dweetHttpGet=dweetHttpGet + String(arrayVariableNames[i]) + "="+ String(arrayVariableValues[i]) + "&";
  }
  dweetHttpGet=dweetHttpGet+" HTTP/1.1\r\n"+
               "Host: " +
                 host +
               "\r\n" +
               "Connection: close\r\n\r\n";
  return dweetHttpGet;    //Our freshly made http string request
}

/*
 * Sends the dweet to dweet.io
 */
void sendDweet(){
  WiFiClient client;
  const int httpPort = 80;

  //connect to dweet.io
  if (!client.connect(host, httpPort)){
    Serial.println("Connection failed.");
    return;
  }
  client.print(getDweetString());
  delay(10); //wait...
  while (client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("Closing connection.");
}

/*
 * Configuration - runs once
 */
void setup() {
  Serial.begin(115200); //enable Serial Monitor
  SUART.begin(9600); //enable SUART Port

  WiFi.begin(ssid, pass);
  connect();
    
  Serial.println(F("Setup ready"));
}

/*
 * Main loop - runs forever
 */
void loop(){

  arrayVariableValues[0] = random(100);
  arrayVariableValues[1] = random(100);

  // display the readings to your serial monitor (locally)
  Serial.print("Sending dweet to ");
  Serial.print(host);
  Serial.print("/follow/");
  Serial.print(thingName);
  Serial.println();

  sendDweet(); //send data to dweet.io
  delay(1000); //refresh rate (send a dweet every 1 second)
}
