#include <Arduino.h>
#include <M5AtomS3.h>
#include <BLE_Kit4C3.h>
#include "./Tasks/Main_Task/Main_Task.h"
#include "./Tasks/UserIO_Task/UserIO_Task.h"

TaskHandle_t thp[2];

BLE_Peripheral ble("AtomS3 Debugger");
const uint8_t DISPLAY_BUTTON = 41;

void setup() {
  Serial2.begin(115200, SERIAL_8N1, 39, 38);
  Serial.begin(115200); //Start Serial monitor in 115200
  Serial.println("Start BLE Peripheral");
  ble.enableDebugMode();
  ble.init();
  Serial.println("Bluetooth Device is Ready to Pair");

  pinMode(DISPLAY_BUTTON, INPUT_PULLUP);
  
  xTaskCreatePinnedToCore(main_task, "main_task", 4096, NULL, 1, &thp[0], 0); 
  //xTaskCreatePinnedToCore(UserIO, "UserIO", 4096, NULL, 2, &thp[1], 0);
}

void loop(){}