#ifndef BLUETOOTH_COMMUNICATION_H
#define BLUETOOTH_COMMUNICATION_H

#include <BluetoothSerial.h>

extern BluetoothSerial SerialBT;

void setupBluetooth();
void sendBluetoothData(float roomTemperature, float humidity, float bodyTemperature, float heartRate);

#endif
