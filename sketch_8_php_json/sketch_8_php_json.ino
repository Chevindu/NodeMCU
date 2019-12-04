#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid     = "EdimaxADSL";  
const char* password = "skoop@123";

const char* host     = "fypwebui.herokuapp.com"; // Your domain, no slashes (ex: www.nyl.io)
String path          = "/light.json"; // path to light.json beginning with slash (ex: /esptest/light.json) 
//const int pin        = 2;

void setup() {  
  pinMode(LED_BUILTIN, OUTPUT); 
  Serial.begin(115200);

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");  
  Serial.println("IP address: " + WiFi.localIP());
}

void loop() {  
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\n\r\n");

  delay(500); // wait for server to respond

  // read response
  String section="header";
  while(client.available()){
    String line = client.readStringUntil('\r');
    // Serial.print(line);
    // we’ll parse the HTML body here
    if (section=="header") { // headers..
      Serial.print(".");
      if (line=="\n") { // skips the empty space at the beginning 
        section="json";
      }
    }
    else if (section=="json") {  // print the good stuff
      section="ignore";
      String result = line.substring(1);
      
      // Parse JSON
      int size = result.length() + 1;
      char json[size];
      result.toCharArray(json, size);
      StaticJsonDocument<200> jsonBuffer;
      //JsonObject& json_parsed = jsonBuffer.parseObject(json);
      JsonObject& json_parsed = deserializeJson(jsonBuffer,json);
      if (!json_parsed.success())
      {
        Serial.println("parseObject() failed");
        return;
      }

      // Make the decision to turn off or on the LED
      if (strcmp(json_parsed["light"], "on") == 0) {
        digitalWrite(LED_BUILTIN, HIGH); 
        Serial.println("LED ON");
      }
      else {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("led off");
      }
    }
  }
  Serial.print("closing connection. ");
}
