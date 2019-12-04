#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

using namespace websockets;

const char* ssid = "Chevva";
const char* password = "bacon123";
const char* websockets_connection_string = "wss://fypwebui.herokuapp.com:3000/";
const char herokuapp_com_ssl_fingerprint[] PROGMEM = "08 3B 71 72 02 43 6E CA ED 42 86 93 BA 7E DF 81 C4 BC 62 30";

WebsocketsClient client;
WiFiClientSecure wificlient;

// Allocate the JSON document
String json_string;

// Use arduinojson.org/v6/assistant to compute the capacity.
const size_t capacity = JSON_OBJECT_SIZE(2);
DynamicJsonDocument doc(capacity);

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

void setup() {
  Serial.begin(115200);
  // Connect to wifi
  WiFi.begin(ssid, password);
  
  // Wait for wifi router
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(1000);
  }

  bool connectivity = wificlient.connect("fypwebui.herokuapp.com", 39631);
  Serial.println(connectivity);

  // Check if router is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No Wifi!");
    return;
  } else {
    Serial.println("Connected!");
  }

//  client.setInsecure();

  // run callback when messages are received
  client.onMessage(onMessageCallback);

  // run callback when events are occuring
  client.onEvent(onEventsCallback);

  // Before connecting, set the ssl fingerprint of the server
  client.setFingerprint(herokuapp_com_ssl_fingerprint);

  Serial.println("Connected to Wifi, Connecting to server.");

  // Connect to server
  bool connected = client.connect(websockets_connection_string);

  // Wait for the websockets server
  for (int i = 0; i < 10 && !connected; i++) {
    Serial.print(".");
    delay(1000);
  }

  if (connected) {
    Serial.println("Connected to Websocket server!");

    // Add values in the document
    doc["from"] = "robot";
    doc["body"] = "Hello Server";

    // Generate the minified JSON and send it to the Serial port.
    serializeJson(doc, json_string);
    Serial.println("json_string value: " + json_string);
    client.send(json_string);

  } else {
    Serial.println("Not Connected!");
  }

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
  delay(500);
}
