#include "Main_Task.h"

void main_task(void *pvParameters){
  // 3つのモードを順に回す
  const DebugMode SERIAL_MODE = DebugMode::SERIAL_MODE;
  const DebugMode BLUETOOTH_MODE = DebugMode::BLUETOOTH_MODE;
  const DebugMode ALL_MODE = DebugMode::ALL_MODE;
  while (1){
    // Serial Mode
    Serial.println("Serial Mode");
    xQueueSendToBack(xModeQueue, &SERIAL_MODE, portMAX_DELAY);
    syncSerial(DebugMode::SERIAL_MODE);
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      std::vector<char> recievedData = GET_SERIAL_DATA(DebugMode::SERIAL_MODE);
      if (digitalRead(DISPLAY_BUTTON) == LOW) break;
      SEND_SERIAL(DebugMode::SERIAL_MODE, recievedData);
      SEND_ANOTHER_TASK(recievedData);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);

    // Bluetooth Mode
    Serial.println("Bluetooth Mode");
    xQueueSendToBack(xModeQueue, &BLUETOOTH_MODE, portMAX_DELAY);
    syncSerial(DebugMode::BLUETOOTH_MODE);
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      std::vector<char> recievedData = GET_SERIAL_DATA(DebugMode::BLUETOOTH_MODE);
      if (digitalRead(DISPLAY_BUTTON) == LOW) break;
      SEND_SERIAL(DebugMode::BLUETOOTH_MODE, recievedData);
      SEND_ANOTHER_TASK(recievedData);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);

    // All Mode
    Serial.println("All Mode");
    xQueueSendToBack(xModeQueue, &ALL_MODE, portMAX_DELAY);
    syncSerial(DebugMode::ALL_MODE);
    while (digitalRead(DISPLAY_BUTTON) == HIGH){
      std::vector<char> recievedData = GET_SERIAL_DATA(DebugMode::ALL_MODE);
      if (digitalRead(DISPLAY_BUTTON) == LOW) break;
      SEND_SERIAL(DebugMode::ALL_MODE, recievedData);
      SEND_ANOTHER_TASK(recievedData);
    }
    while (digitalRead(DISPLAY_BUTTON) == LOW) delay(1);
  }
}

static void syncSerial(const DebugMode& mode){ // 改行コードを受け取るまで待つ
  do {
    while (Serial2.available() == 0 && digitalRead(DISPLAY_BUTTON) == HIGH) {
      recieveBLEandSerial(mode);
      delay(1);
    }
  } while (Serial2.read() != 0x0A && digitalRead(DISPLAY_BUTTON) == HIGH);
}

static std::vector<char> GET_SERIAL_DATA(const DebugMode& mode){ // 改行コードを受け取るまでデータを蓄積する
  std::vector<char> recievedData;
  do {
    while (Serial2.available() == 0 && digitalRead(DISPLAY_BUTTON) == HIGH) {
      recieveBLEandSerial(mode);
      delay(1);
    }
    if (digitalRead(DISPLAY_BUTTON) == LOW) break;
    recievedData.push_back(Serial2.read());
  } while (recievedData.back() != 0x0A);
  char tmp = 0x00;
  xQueueSendToBack(xSerial2RecievedFlagQueue, &tmp, portMAX_DELAY);
  return recievedData;
}

static void SEND_SERIAL(const DebugMode& mode, const std::vector<char>& charArray){
  char* sendArray = new char[charArray.size()];
  std::copy(charArray.begin(), charArray.end(), sendArray);
  if (mode == DebugMode::SERIAL_MODE || mode == DebugMode::ALL_MODE){ // シリアルに送信
    Serial.write(sendArray, charArray.size());

    recieveBLEandSerial(DebugMode::SERIAL_MODE);
  }
  if (mode == DebugMode::BLUETOOTH_MODE || mode == DebugMode::ALL_MODE){ // BLEで送信
    if (ble.checkConnection()) {
      ble.write(sendArray, charArray.size());

      recieveBLEandSerial(DebugMode::BLUETOOTH_MODE);
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

static void recieveBLEandSerial(const DebugMode& mode){
  if (mode == DebugMode::SERIAL_MODE || mode == DebugMode::ALL_MODE){
    if (Serial.available() != 0){ // 受信した場合
      WrittenBy hoge = WrittenBy::Serial0;
      xQueueSendToBack(xSerial2WriteFlagQueue, &hoge, portMAX_DELAY);
      while (Serial.available() != 0) { 
        char data = Serial.read();
        Serial2.write(data);
      } 
    }
  }
  if (mode == DebugMode::BLUETOOTH_MODE || mode == DebugMode::ALL_MODE){
    if (ble.available() != 0) {
      WrittenBy hoge = WrittenBy::BLE;
      xQueueSendToBack(xSerial2WriteFlagQueue, &hoge, portMAX_DELAY);
      while (ble.available() != 0) { // 受信した場合
        char data = ble.read();
        Serial.write(data);
        Serial2.write(data);
      }
    }
  }
}
