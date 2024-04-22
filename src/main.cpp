#include <Arduino.h>
#include "./Tasks/Main_Task/Main_Task.h"
#include "./Tasks/UserIO_Task/UserIO_Task.h"

TaskHandle_t thp[2];
QueueHandle_t xSerialBox;

struct DATA_SET {
  int16_t IMU_X;
  int16_t IMU_Y;
  int16_t IMU_Z;
  uint16_t LEFT_SPEED;
  uint16_t RIGHT_SPEED;
};

void setup() {
  xSerialBox = xQueueCreate(10, sizeof(DATA_SET));
  xTaskCreatePinnedToCore(main_task, "main_task", 4096, NULL, 3, &thp[0], 0); 
  xTaskCreatePinnedToCore(UserIO, "UserIO", 4096, NULL, 5, &thp[1], 0);
}

void loop(); // ｷｮﾑﾘ