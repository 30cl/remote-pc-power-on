#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "config.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

ESP8266WebServer server(80);

void btnPress(int pin, int time) {
  digitalWrite(pin, HIGH);
  delay(time);
  digitalWrite(pin, LOW);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(STATE_PIN, INPUT);

  digitalWrite(LED_BUILTIN, LOW); // led is reversed on the D1Mini...

  WiFi.begin(ssid, password);

  server.on("/status", HTTP_GET, handleGetStatus);
  server.on("/power-on", HTTP_GET, handleGetPowerOn);
  server.on("/power-off", HTTP_GET, handleGetPowerOff);
  server.on("/power-toggle", HTTP_GET, handleGetPowerToggle);
  server.on("/force-off", HTTP_GET, handleGetForceOff);
  server.on("/reset", HTTP_GET, handleGetReset);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    return;
  }

  server.handleClient();
  delay(10);
}

void handleGetStatus() {
  String result = "{\"status\": \"";
  result += (digitalRead(STATE_PIN) ? "on" : "off");
  result += "\"}\n";
  server.send(200, "application/json", result);
}

void handleGetPowerOn() {
  if (digitalRead(STATE_PIN) == LOW) {
    btnPress(POWER_PIN, SHORT_PRESS);
  }
  handleGetStatus();
}

void handleGetPowerOff() {
  if (digitalRead(STATE_PIN) == HIGH) {
    btnPress(POWER_PIN, SHORT_PRESS);
  }
  handleGetStatus();
}

void handleGetPowerToggle() {
  btnPress(POWER_PIN, SHORT_PRESS);
  handleGetStatus();
}

void handleGetForceOff() {
  if (digitalRead(STATE_PIN) == HIGH) {
    btnPress(POWER_PIN, LONG_PRESS);
  }
  handleGetStatus();
}

void handleGetReset() {
  btnPress(RESET_PIN, SHORT_PRESS);
  handleGetStatus();
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}
