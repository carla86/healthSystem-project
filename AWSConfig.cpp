#include "AWSConfig.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"  

WiFiClientSecure net;
PubSubClient mqttClient(net);

void connectAWS() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Wi-Fi Connected!");

  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  mqttClient.setServer(AWS_IOT_ENDPOINT, 1883);
  mqttClient.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!mqttClient.connect("ESP32HealthMonitor")) {
    Serial.print(".");
    delay(100);
  }

  if (!mqttClient.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  mqttClient.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}
