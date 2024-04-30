#pragma once
#include <Arduino.h>
#include <BLE_Kit4C3.h>
#include "../QUEUE_DATA_SET.h"

/*
 * Seral2からデータを受け取って
 * * Serial
 * * Queue
 * * Bluetooth
 * に流す
*/

extern const uint8_t DISPLAY_BUTTON;
extern BLE_Peripheral ble;

void main_task(void *pvParameters);

static void syncSerial();

static std::vector<char> GET_SERIAL_DATA();

static void SEND_ANOTHER_TASK(std::vector<char>& charArray);

static void SEND_SERIAL(DebugMode mode, std::vector<char>& charArray);