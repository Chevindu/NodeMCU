#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

using namespace websockets;

const char* ssid = "Chevva"; //Enter SSID
const char* password = "bacon123"; //Enter Password
const char* websockets_server_host = "18.139.227.157"; //Enter server adress
const uint16_t websockets_server_port = 8000; // Enter server port

WebsocketsClient client;

bool wifi_connected;
bool ws_connected;
String json_string;
String json_string2;
const size_t capacity = JSON_OBJECT_SIZE(2);

DynamicJsonDocument first(capacity);
DynamicJsonDocument body(capacity);

void setup() {
  Serial.begin(115200);

  Serial.println("Connecting to WiFi.");

  connectWiFi();

  Serial.println("Connected to WiFi, Connecting to server.");

  connectWebSockets();

    body["from"] = "robot";
    body["body"] = "Hello Server";

serializeJson(body, json_string2);


}

void loop() {

  // let the websockets client check for incoming messages
  if (client.available()) {
    client.poll();
    Serial.println("WS available");
  }
  else {
    Serial.println("WS not available");
  }

  if (ws_connected) {

    Serial.println("json_string value: " + json_string2);
    client.send(json_string2);
  }
  delay(2000);

}

void connectWiFi() {
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait for wifi router
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(1000);
  }

  // Check if router is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No Wifi!");
    wifi_connected = true;
    return;
  } else {
    Serial.println("Connected!");
  }

}

void connectWebSockets() {

  // try to connect to Websockets server
  ws_connected = client.connect(websockets_server_host, websockets_server_port, "/");

  // Wait for the websockets server
  for (int i = 0; i < 10 && !ws_connected; i++) {
    Serial.print(".");
    delay(1000);
  }

  if (ws_connected) {
    Serial.println("Connected!");


    // Setup Callbacks
    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);


    // Add values in the document
    first["from"] = "robot";
    first["body"] = "init_message";

    // Generate the minified JSON and send it to the Serial port.
    serializeJson(first, json_string);
    client.send(json_string);
  } else {
    Serial.println("Not Connected!");
  }
}

void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Got Message: ");
  Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}
