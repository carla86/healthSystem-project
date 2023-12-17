#ifndef AWSCONFIG_H
#define AWSCONFIG_H

#include <Arduino.h>

extern const char AWS_IOT_ENDPOINT[];
extern const char AWS_CERT_CA[];
extern const char AWS_CERT_CRT[];
extern const char AWS_CERT_PRIVATE[];

void connectAWS();
void messageHandler(char* topic, byte* payload, unsigned int length);

#endif 
 
