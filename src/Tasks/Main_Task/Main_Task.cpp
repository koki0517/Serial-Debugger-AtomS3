#include "Main_Task.h"

void main_task(void *pvParameters){
  // 3つのモードを順に回す
  syncSerial();
  while (1){
    // Serial Mode
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      GET_SEND_SERAL_DATA(DebugMode::SERIAL_MODE);
      delay(1);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);

    // Bluetooth Mode
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      GET_SEND_SERAL_DATA(DebugMode::BLUETOOTH_MODE);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);

    // All Mode
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      GET_SEND_SERAL_DATA(DebugMode::ALL_MODE);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);
  }
}

static void syncSerial(){
  do {
    while (Serial2.available() == 0) delay(1);
  } while (Serial2.read() != 0x0A);
}

static void GET_SEND_SERAL_DATA(DebugMode mode){
  std::vector<char> recievedData;
  do {
    while (Serial2.available() == 0) delay(1);
    recievedData.push_back(Serial2.read());
  } while (recievedData.back() != 0x0A);
  char* charArray = new char[recievedData.size()];
  std::copy(recievedData.begin(), recievedData.end(), charArray);
  Serial.write(charArray, recievedData.size());
  delete[] charArray;
}