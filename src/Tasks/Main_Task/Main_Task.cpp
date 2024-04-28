#include "Main_Task.h"

void main_task(void *pvParameters){
  // 3つのモードを順に回す
  while (1){
    // Serial Mode
    Serial.println("Serial Mode");
    syncSerial();
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      std::vector<char> recievedData = GET_SERIAL_DATA();
      if (digitalRead(DISPLAY_BUTTON) == LOW) break;
      SEND_SERIAL(DebugMode::SERIAL_MODE, recievedData);
      delay(1);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);

    // Bluetooth Mode
    Serial.println("Bluetooth Mode");
    syncSerial();
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      GET_SERIAL_DATA();
      delay(1);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);

    // All Mode
    Serial.println("All Mode");
    syncSerial();
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      std::vector<char> recievedData = GET_SERIAL_DATA();
      if (digitalRead(DISPLAY_BUTTON) == LOW) break;
      SEND_SERIAL(DebugMode::SERIAL_MODE, recievedData);
      delay(1);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);
  }
}

static void syncSerial(){
  do {
    while (Serial2.available() == 0 && digitalRead(DISPLAY_BUTTON) == HIGH) delay(1);
  } while (Serial2.read() != 0x0A);
}

static std::vector<char> GET_SERIAL_DATA(){
  std::vector<char> recievedData;
  do {
    while (Serial2.available() == 0 && digitalRead(DISPLAY_BUTTON) == HIGH) delay(1);
    if (digitalRead(DISPLAY_BUTTON) == LOW) break;
    recievedData.push_back(Serial2.read());
  } while (recievedData.back() != 0x0A);
  return recievedData;
}

static void SEND_SERIAL(DebugMode mode, std::vector<char>& charArray){
  char* sendArray = new char[charArray.size()];
  std::copy(charArray.begin(), charArray.end(), sendArray);
  if (mode == DebugMode::SERIAL_MODE || mode == DebugMode::ALL_MODE){
    Serial.write(sendArray, charArray.size());
  }
  delete[] sendArray;
}