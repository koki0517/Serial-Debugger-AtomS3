#include <Arduino.h>
#include <BLE_Kit4C3.h>
#include <cstring>
#include "./Tasks/Main_Task.h"

void initDisplay();

TaskHandle_t thp[1];
QueueHandle_t xQueue, xModeQueue, xSerial2RecievedFlagQueue, xSerial2WriteFlagQueue;

M5Canvas canvas(&M5.Lcd);
M5Canvas canvasSerial(&M5.Lcd);
M5Canvas canvasBluetooth(&M5.Lcd);
M5Canvas canvasAll(&M5.Lcd);
BLE_Peripheral ble("AtomS3 BLE Debugger");
const uint8_t DISPLAY_BUTTON = 41;
unsigned long lastRecievedTime, lastWroteTime;
bool isGreenCircle = false, wroteSerial2 = false;

void setup() {
  auto cfg = M5.config(); // 本体初期設定
  AtomS3.begin(cfg);

  Serial2.begin(115200, SERIAL_8N1, 39, 38);
  Serial.begin(115200); //Start Serial monitor in 115200
  Serial.println("Start BLE Peripheral");
  ble.enableDebugMode();
  ble.init();
  Serial.println("Bluetooth Device is Ready to Pair");

  initDisplay();

  pinMode(DISPLAY_BUTTON, INPUT_PULLUP);
  xQueue = xQueueCreate(10, sizeof(QUEUE_DATA_SET));
  xModeQueue = xQueueCreate(10, sizeof(DebugMode));
  xSerial2RecievedFlagQueue = xQueueCreate(10, sizeof(char));
  xSerial2WriteFlagQueue = xQueueCreate(10, sizeof(WrittenBy));
  lastRecievedTime = millis();
  
  xTaskCreatePinnedToCore(main_task, "main_task", 8192, NULL, 1, &thp[0], 0); 
}

void loop(){
  BaseType_t result;

  QUEUE_DATA_SET recievedData;
  result = xQueueReceive(xQueue, &recievedData, 1);
  if (result == pdTRUE){
    if (std::strstr(recievedData.data, "*IMU X") != nullptr) {
      Serial.println("x");
    }
    else if (std::strstr(recievedData.data, "*IMU Y") != nullptr) {
      Serial.println("y");
    }
    else if (std::strstr(recievedData.data, "*IMU Z") != nullptr) {
      Serial.println("z");
    }
    else if (std::strstr(recievedData.data, "*Left Motor") != nullptr) {
      Serial.println("left");
    }
    else if (std::strstr(recievedData.data, "*Right Motor") != nullptr) {
      Serial.println("right");
    }
  }
  
  DebugMode recievedMode;
  result = xQueueReceive(xModeQueue, &recievedMode, 1);
  if (result == pdTRUE){
    switch (recievedMode){
      case DebugMode::SERIAL_MODE:
        canvasSerial.pushSprite(0,26);
        break;
      case DebugMode::BLUETOOTH_MODE:
        canvasBluetooth.pushSprite(0,26);
        break;
      case DebugMode::ALL_MODE:
        canvasAll.pushSprite(0,26);
        break;
    }
  }

  char tmp;
  result = xQueueReceive(xSerial2RecievedFlagQueue, &tmp, 1);
  if (result == pdTRUE){
    lastRecievedTime = millis();
    if (!isGreenCircle){
      isGreenCircle = true;
      M5.Lcd.fillCircle(100, 36, 5, TFT_GREEN);
    }
  } else if ((millis() - lastRecievedTime) > 1000 && isGreenCircle){ // 1s以上受け取ってない
    M5.Lcd.fillCircle(100, 36, 5, TFT_RED);
    isGreenCircle = false;
  }

  WrittenBy recievedAddr;
  result = xQueueReceive(xSerial2WriteFlagQueue, &recievedAddr, 1);
  if (result == pdTRUE) {
    lastWroteTime = millis();
    wroteSerial2 = true;
    if (recievedAddr == WrittenBy::BLE){
      M5.Lcd.fillCircle(115, 36, 5, TFT_GREEN);
    } else { // == Serial0
      M5.Lcd.fillCircle(115, 36, 5, TFT_GOLD);
    }
  } else if (wroteSerial2 && (millis() - lastWroteTime) > 2000){
    M5.Lcd.fillCircle(115, 36, 5, TFT_RED);
  }
  delay(1);
}

void initDisplay(){
  // 液晶初期化
  M5.Lcd.init();                         // 初期化
  M5.Lcd.setTextWrap(false);             // テキストが画面からはみ出した時の折り返し無し
  M5.Lcd.clear(M5.Lcd.color565(30, 30, 100));  // 背景色
  M5Canvas canvasTitle(&M5.Lcd), canvasTitleBottom(&M5.Lcd);
  canvasTitle.setColorDepth(16);     // カラーモード設定(16bit)
  canvasTitle.setTextWrap(false);    // テキストが画面からはみ出した時の折り返し無し
  canvasTitle.createSprite(128, 26); // メモリ描画(カウント値表示)領域を設定(X, Y)

  // 初期画面
  canvasTitle.fillRect(0, 0, 128, 26, TFT_WHITE);       // タイトルエリア背景
  canvasTitle.setTextColor(canvas.color565(50,50,255));
  canvasTitle.drawString("B", 4, 0, &fonts::efontJA_24);
  canvasTitle.drawString("L", 15, 0, &fonts::efontJA_24);
  canvasTitle.drawString("E", 25, 0, &fonts::efontJA_24);
  canvasTitle.setTextColor(canvas.color565(255,50,50));
  canvasTitle.drawString("D", 37, 0, &fonts::efontJA_24);
  canvasTitle.drawString("e", 48, 0, &fonts::efontJA_24);
  canvasTitle.drawString("b", 59, 0, &fonts::efontJA_24);
  canvasTitle.drawString("u", 70, 0, &fonts::efontJA_24);
  canvasTitle.drawString("g", 81, 0, &fonts::efontJA_24);
  canvasTitle.drawString("g", 92, 0, &fonts::efontJA_24);
  canvasTitle.drawString("e", 103, 0, &fonts::efontJA_24);
  canvasTitle.drawString("r", 114, 0, &fonts::efontJA_24);
  canvasTitle.pushSprite(0,0);
  M5.Lcd.drawFastHLine(0, 46, 128, TFT_WHITE); // モードのバー

  M5.Lcd.fillCircle(100, 36, 5, TFT_RED);
  M5.Lcd.fillCircle(115, 36, 5, TFT_RED);

  // メモリ描画(カウント値表示)領域初期化
  canvas.setColorDepth(16);     // カラーモード設定(16bit)
  canvas.setTextWrap(false);    // テキストが画面からはみ出した時の折り返し無し
  canvas.createSprite(128, 102); // メモリ描画(カウント値表示)領域を設定(X, Y)

  canvasSerial.setColorDepth(16);
  canvasBluetooth.setColorDepth(16);
  canvasAll.setColorDepth(16);
  canvasSerial.setTextWrap(false);
  canvasBluetooth.setTextWrap(false);
  canvasAll.setTextWrap(false);
  canvasSerial.createSprite(91, 20);
  canvasBluetooth.createSprite(91, 20);
  canvasAll.createSprite(91, 20);
  canvasSerial.fillRect(0, 0, 91, 20, canvasSerial.color565(0,22,90));
  canvasBluetooth.fillRect(0, 0, 91, 20, canvasSerial.color565(0,22,90));
  canvasAll.fillRect(0, 0, 91, 20, canvasSerial.color565(0,22,90));
  canvasSerial.setTextColor(TFT_CYAN);
  canvasBluetooth.setTextColor(TFT_CYAN);
  canvasAll.setTextColor(TFT_CYAN);
  canvasSerial.drawString("Ser", 4, 1, &fonts::efontJA_16);
  canvasSerial.drawString("ial", 26, 1, &fonts::efontJA_16);
  canvasSerial.drawString("Mode", 53, 1, &fonts::efontJA_16);
  canvasBluetooth.drawString("BLE Mode", 4, 1, &fonts::efontJA_16);
  canvasAll.drawString("ALL Mode", 4, 1, &fonts::efontJA_16);
}