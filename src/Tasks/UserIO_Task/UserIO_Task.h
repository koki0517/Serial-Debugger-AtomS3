#pragma once
#include <Arduino.h>
#include "../QUEUE_DATA_SET.h"

extern QueueHandle_t xRobotStatus;

void UserIO(void *pvParameters);