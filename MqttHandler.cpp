#include "MqttHandler.h"
#include "WiFiConfig.h"
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>

extern WiFiClientSecure net;  // it´s coming from the main (extern)
extern PubSubClient mqttClient;  

extern float humidity;
extern float roomTemperature;
extern float bodyTemperature;
extern float heartRate;

void connectAWS() {
  Serial.println("Connecting to Wi-Fi...");
  setupWiFi();  
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

void publishMessage() {
  StaticJsonDocument<200> doc;
  doc["timestamp"] = millis();
  doc["humidity"] = humidity;
  doc["roomTemperature"] = roomTemperature;
  doc["bodyTemperature"] = bodyTemperature;
  doc["heartRate"] = heartRate;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  mqttClient.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, humidity);
  Blynk.virtualWrite(VIRTUAL_PIN_ROOM_TEMP, roomTemperature);
  Blynk.virtualWrite(VIRTUAL_PIN_BODY_TEMP, bodyTemperature);
  Blynk.virtualWrite(VIRTUAL_PIN_HEART_RATE, heartRate);
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}
