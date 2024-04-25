#pragma once
#include <Arduino.h>
#include "../QUEUE_DATA_SET.h"
#include <unordered_map>
/*
 * Seral2からデータを受け取って
 * * Serial
 * * Queue
 * * Bluetooth
 * に流す
*/

extern uint8_t DISPLAY_BUTTON = 41;

enum class DebugMode{
  SERIAL_MODE,
  BLUETOOTH_MODE,
  ALL_MODE,
};

void main_task(void *pvParameters);

static void GET_SERAL_DATA();

static void SEND_ANOTHER_TASK();

static void SNED_SERIAL();

static void SEND_BLUETOOTH();