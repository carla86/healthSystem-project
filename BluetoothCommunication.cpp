#include "BluetoothCommunication.h"

void setupBluetooth() {
  SerialBT.begin("ESP32HealthMonitor");
}

void sendBluetoothData(float roomTemperature, float humidity, float bodyTemperature, float heartRate) {
  String data = "Room Temp: " + String(roomTemperature) +
                ", Humidity: " + String(humidity) +
                ", Body Temp: " + String(bodyTemperature) +
                ", Heart Rate: " + String(heartRate);

  SerialBT.println(data);
}


