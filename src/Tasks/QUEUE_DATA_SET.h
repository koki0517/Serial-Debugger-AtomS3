#pragma once
#include <Arduino.h>

enum class DATA_ID{
  BASIC_STATUS = 0x00,
};

struct BASIC_STATUS_SET {
  int16_t IMU_X;
  int16_t IMU_Y;
  int16_t IMU_Z;
  uint16_t LEFT_SPEED;
  uint16_t RIGHT_SPEED;
};