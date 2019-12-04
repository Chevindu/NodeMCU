#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "telepresence_keys.h"

SoftwareSerial SUART(4, 5); //STX=Dpin-D1; SRX=DPin-D2

void connectWiFi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP());  
}

void setup() {
  // Initialize Serial port
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200); //Serial port to Monitor
  SUART.begin(9600);    //Serial port to Arduino

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);
  connectWiFi();

  Serial.println(F("Connecting..."));

}

void loop() {
  // Connect to HTTP server
  WiFiClient client;
  client.setTimeout(10000);
  if (!client.connect("fypwebui.herokuapp.com", 80)) {
    Serial.println(F("Connection failed"));
    return;
  }

  // Send HTTP request
  client.println(F("GET /light.json HTTP/1.0"));
  client.println(F("Host: fypwebui.herokuapp.com"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Extract values
  Serial.println(F("Response:"));     
  
  // Make the decision to turn off or on the LED
      if (strcmp(doc["light"].as<char*>(), "on") == 0) {
        digitalWrite(LED_BUILTIN, LOW); 
        Serial.print("LED ON");
        SUART.println("Y");
        Serial.println();
      }
      else {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print("led off");
        SUART.println("N");
        Serial.println();
      }

  // Disconnect
  client.stop();

}
