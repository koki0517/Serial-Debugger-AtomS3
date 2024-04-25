#include <Arduino.h>
#include <M5AtomS3.h>
#include <BLE_Kit4C3.h>

BLE_Peripheral ble("BLE_Kit4C3 Peripheral");
const uint8_t DISPLAY_BUTTON = 41;

void setup() {
  Serial2.begin(9600, SERIAL_8N1, 39, 38);
  Serial.begin(115200); //Start Serial monitor in 115200
  Serial.println("Start BLE Peripheral");
  ble.enableDebugMode();
  ble.init();
  Serial.println("Bluetooth Device is Ready to Pair");

  pinMode(DISPLAY_BUTTON, INPUT_PULLUP);
}

void loop() {
  if (ble.checkConnection()) {
    static unsigned long flagTimer = millis();
    if (millis() - flagTimer > 1000) {
      char sendDataArr[] = "Hello world!";
      int dataSize = sizeof(sendDataArr) / sizeof(sendDataArr[0]);

      ble.write(sendDataArr, dataSize);
      Serial.println("Send Data to Central");

      flagTimer = millis();
    }

    while (ble.available() != 0) {
      char data = ble.read();
      Serial.write(data);
    }
  }
}