#pragma once
#include <Arduino.h>
#include "../QUEUE_DATA_SET.h"

/*
 * Seral2からデータを受け取って
 * * Serial
 * * Queue
 * * Bluetooth
 * に流す
*/

extern const uint8_t DISPLAY_BUTTON;

void main_task(void *pvParameters);

static void syncSerial();

static void GET_SEND_SERAL_DATA(DebugMode mode);

static void SEND_ANOTHER_TASK();

static void SNED_SERIAL(uint8_t dataID, std::vector<char>& byteArray);
static void SEND_BLUETOOTH(char dataID, std::vector<std::vector<char>>& byteArray);