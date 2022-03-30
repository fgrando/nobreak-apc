#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include "WIFI_SSID.h" // defines WIFI_SSID as wifi ssid
#include "WIFI_PASS.h" // defines WIFI_PASS as wifi password

#ifndef STASSID
#define STASSID WIFI_SSID
#define STAPSK  WIFI_PASS
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const int SENSOR_PIN = A0;

const int SSID_START = 1;
const int PASS_START = 30;
unsigned int COUNTER = 0;

ESP8266WebServer server(80);

void handleSSID() {
  server.send(200, "text/plain", ssid);
}

void handlePASS() {
  server.send(200, "text/plain", password);
}

void handleRoot() {
  COUNTER++;
  int value = analogRead(SENSOR_PIN);
  String message = String(COUNTER) + ": sensor = " + String(value);
  server.send(200, "text/plain", message);
}

void handleNotFound() {
  server.send(404, "text/plain", "not found");
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("nobreak")) {
    Serial.println("MDNS responder started");
  }
}

void setup(void) {
  pinMode(SENSOR_PIN, INPUT);

  EEPROM.begin(512);
  Serial.begin(115200);
  Serial.println("Connecting wifi");
  Serial.println(ssid);
  Serial.println(password);
  
  setupWiFi();

  server.on("/", handleRoot);
  server.on("/ssid", handleSSID);
  server.on("/pass", handlePASS);
  server.onNotFound(handleNotFound);
  server.begin();
  
  Serial.println("HTTP server started");
}

void loop(void) {  
  server.handleClient();
  MDNS.update();
}
