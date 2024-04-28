#pragma once
#include <Arduino.h>
#include <unordered_map>
#include <vector>
#include <string>

enum class DebugMode{
  SERIAL_MODE,
  BLUETOOTH_MODE,
  ALL_MODE,
};