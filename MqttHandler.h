#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "AWSConfig.h"
#include <ArduinoJson.h>


extern WiFiClientSecure net;
extern PubSubClient mqttClient;

void connectAWS();
void publishMessage();
void messageHandler(char* topic, byte* payload, unsigned int length);

#endif
