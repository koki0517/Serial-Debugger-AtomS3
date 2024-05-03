#include "Main_Task.h"

void main_task(void *pvParameters){
  // 3つのモードを順に回す
  const DebugMode SERIAL_MODE = DebugMode::SERIAL_MODE;
  const DebugMode BLUETOOTH_MODE = DebugMode::BLUETOOTH_MODE;
  const DebugMode ALL_MODE = DebugMode::ALL_MODE;
  while (1){
    // Serial Mode
    Serial.println("Serial Mode");
    syncSerial();
    xQueueSendToBack(xModeQueue, &SERIAL_MODE, portMAX_DELAY);
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      std::vector<char> recievedData = GET_SERIAL_DATA();
      if (digitalRead(DISPLAY_BUTTON) == LOW) break;
      SEND_SERIAL(DebugMode::SERIAL_MODE, recievedData);
      SEND_ANOTHER_TASK(recievedData);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);

    // Bluetooth Mode
    Serial.println("Bluetooth Mode");
    syncSerial();
    xQueueSendToBack(xModeQueue, &BLUETOOTH_MODE, portMAX_DELAY);
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      std::vector<char> recievedData = GET_SERIAL_DATA();
      if (digitalRead(DISPLAY_BUTTON) == LOW) break;
      SEND_SERIAL(DebugMode::BLUETOOTH_MODE, recievedData);
      SEND_ANOTHER_TASK(recievedData);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);

    // All Mode
    Serial.println("All Mode");
    syncSerial();
    xQueueSendToBack(xModeQueue, &ALL_MODE, portMAX_DELAY);
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      std::vector<char> recievedData = GET_SERIAL_DATA();
      if (digitalRead(DISPLAY_BUTTON) == LOW) break;
      SEND_SERIAL(DebugMode::ALL_MODE, recievedData);
      SEND_ANOTHER_TASK(recievedData);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);
  }
}

static void syncSerial(){ // 改行コードを受け取るまで待つ
  do {
    while (Serial2.available() == 0 && digitalRead(DISPLAY_BUTTON) == HIGH) delay(1);
  } while (Serial2.read() != 0x0A);
}

static std::vector<char> GET_SERIAL_DATA(){ // 改行コードを受け取るまでデータを蓄積する
  std::vector<char> recievedData;
  do {
    while (Serial2.available() == 0 && digitalRead(DISPLAY_BUTTON) == HIGH) delay(1);
    if (digitalRead(DISPLAY_BUTTON) == LOW) break;
    recievedData.push_back(Serial2.read());
  } while (recievedData.back() != 0x0A);
  return recievedData;
}

static void SEND_SERIAL(DebugMode mode, const std::vector<char>& charArray){
  char* sendArray = new char[charArray.size()];
  std::copy(charArray.begin(), charArray.end(), sendArray);
  if (mode == DebugMode::SERIAL_MODE || mode == DebugMode::ALL_MODE){ // シリアルに送信
    Serial.write(sendArray, charArray.size());

    while (Serial.available() != 0) { // 受信した場合
      char data = Serial.read();
      Serial2.write(data);
    } 
  }
  if (mode == DebugMode::BLUETOOTH_MODE || mode == DebugMode::ALL_MODE){ // BLEで送信
    if (ble.checkConnection()) {
      ble.write(sendArray, charArray.size());

      while (ble.available() != 0) { // 受信した場合
        char data = ble.read();
        Serial.write(data);
        Serial2.write(data);
      }
    }
  }
  delete[] sendArray;
}

static void SEND_ANOTHER_TASK(const std::vector<char>& charArray){
  if (charArray.front() == 0x2A/* *要素 */) {
    // Queueに送る
    QUEUE_DATA_SET send_data;
    send_data.size = charArray.size();
    std::copy(charArray.begin(), charArray.end(), send_data.data);
    xQueueSendToBack(xQueue, &send_data, portMAX_DELAY);
  }
}