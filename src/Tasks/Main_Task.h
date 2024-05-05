#pragma once
#include <Arduino.h>
#include <M5AtomS3.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <BLE_Kit4C3.h>

/*
 * Seral2からデータを受け取って
 * * Serial
 * * Queue
 * * Bluetooth
 * に流す
*/

extern const uint8_t DISPLAY_BUTTON;
extern BLE_Peripheral ble;
extern QueueHandle_t xQueue, xModeQueue, xSerial2RecievedFlagQueue, xSerial2WriteFlagQueue;

enum class DebugMode :int8_t{
  SERIAL_MODE,
  BLUETOOTH_MODE,
  ALL_MODE,
};

enum class WrittenBy :int8_t{
  BLE,
  Serial0,
};

struct QUEUE_DATA_SET{
  uint8_t size;
  char data[20];
};

void main_task(void *pvParameters);

static void syncSerial();
static std::vector<char> GET_SERIAL_DATA();
static void SEND_ANOTHER_TASK(const std::vector<char>& charArray);
static void SEND_SERIAL(DebugMode mode, const std::vector<char>& charArray);