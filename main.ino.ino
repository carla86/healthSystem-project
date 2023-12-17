#define BLYNK_TEMPLATE_ID "TMPL5jIgLNxfs"
#define BLYNK_TEMPLATE_NAME "Stress Monitoring System"

#include "secrets.h"
#include "WiFiConfig.h"
#include "AWSConfig.h"
#include "MqttHandler.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <MAX30100_PulseOximeter.h>
#include <BlynkSimpleEsp32.h>
#include "FS.h"
#include "LittleFS.h"
#include "BluetoothCommunication.h"

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
#define REPORTING_PERIOD_MS 1000
#define DS18B20 13
#define DHTPIN 4  
#define DHTTYPE DHT11

char auth[] = "R1gvjCDuyu4r4XldugI6vjQjzIwPvwDG";
const char *mqtt_server = "192.168.0.112";

uint8_t DHTPin = 4;
DHT dht(DHTPin, DHTTYPE);
float roomTemperature, humidity, bodyTemperature, heartRate;
uint32_t tsLastReport = 0;

OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);

WiFiClientSecure net = WiFiClientSecure();
PubSubClient mqttClient(net);

PulseOximeter pox;
PulseOximeter heartRateSensor;
BluetoothSerial SerialBT;

#define VIRTUAL_PIN_HUMIDITY        V4
#define VIRTUAL_PIN_ROOM_TEMP       V3
#define VIRTUAL_PIN_BODY_TEMP       V6
#define VIRTUAL_PIN_HEART_RATE      V5

void onBeatDetected() {
  Serial.println("Beat Detected!");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  setupBluetooth();

  pinMode(19, OUTPUT);
  pinMode(DHTPin, INPUT);
  
  if (!LittleFS.begin()) {
    Serial.println("LittleFS initialization failed. Formatting...");
    LittleFS.format();
    Serial.println("LittleFS formatted successfully.");
  } else {
    Serial.println("LittleFS initialized successfully.");
  }

  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);

  connectAWS();

  Serial.print("Initializing Pulse Oximeter..");
  if (!pox.begin() || !heartRateSensor.begin()) {
    Serial.println("FAILED");
    for (;;)
      ;
  } else {
    // Set the IR LED current for the MAX30100
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    Serial.println("SUCCESS");
    pox.setOnBeatDetectedCallback(onBeatDetected);
    heartRateSensor.setOnBeatDetectedCallback(onBeatDetected);
  }
}

void loop() {
  Blynk.run();

  sensors.requestTemperatures();  // Request temperature readings from DS18B20
  heartRate = heartRateSensor.getHeartRate();
  bodyTemperature = sensors.getTempCByIndex(0);  //  DS18B20  for body temperature

  roomTemperature = dht.readTemperature();
  humidity = dht.readHumidity();


  if (!isnan(roomTemperature) && !isnan(humidity)) {
    Serial.print("Room Temperature: ");
    Serial.println(roomTemperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
  } else {
    Serial.println("Failed to read from DHT sensor!");
}

  Serial.print("Heart Rate: ");
  Serial.println(heartRate);
  Serial.print("Body Temperature: ");
  Serial.println(bodyTemperature);
  Serial.print("Room Temperature: ");
  Serial.println(roomTemperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);

  mqttClient.loop();

  publishMessage();

  sendBluetoothData(roomTemperature, humidity, bodyTemperature, heartRate);
}
