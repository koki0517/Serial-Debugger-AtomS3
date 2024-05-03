#include <Arduino.h>
#include <M5AtomS3.h>
#include <BLE_Kit4C3.h>
#include <cstring>
#include "./Tasks/Main_Task.h"

void initDisplay();

TaskHandle_t thp[1];
QueueHandle_t xQueue, xModeQueue;

M5Canvas canvas(&M5.Lcd);
BLE_Peripheral ble("AtomS3 Debugger");
const uint8_t DISPLAY_BUTTON = 41;

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
  
  xTaskCreatePinnedToCore(main_task, "main_task", 8192, NULL, 1, &thp[0], 0); 
}

void loop(){
  QUEUE_DATA_SET recievedData;
  xQueueReceive(xQueue, &recievedData, portMAX_DELAY);

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
  
  DebugMode recievedMode;
  BaseType_t result;
  result = xQueueReceive(xModeQueue, &recievedMode, 1);
  if (result == pdTRUE){
    switch (recievedMode){
      case DebugMode::SERIAL_MODE:
        Serial.println("#Serial Mode");
        break;
      case DebugMode::BLUETOOTH_MODE:
        Serial.println("#Bluetooth Mode");
        break;
      case DebugMode::ALL_MODE:
        Serial.println("#All Mode");
        break;
    }
  }
  delay(1);

  canvas.setColorDepth(16);                        // カラーモード設定(16bit)
  canvas.createSprite(M5.Lcd.width(), M5.Lcd.height());  // canvas1サイズ（メモリ描画領域）設定（画面サイズに設定）
  canvas.fillScreen(TFT_BLACK);                    // 背景色（透明）
  canvas.fillCircle(64, 64, 63, TFT_LIGHTGRAY);    // 塗り潰し円
  canvas.fillCircle(64, 64, 59, TFT_ORANGE);       // 塗り潰し円
  canvas.setTextColor(TFT_WHITE);                  // 文字色
  canvas.drawCentreString("ロジカラ", 64, 37, &fonts::lgfxJapanGothicP_28);  // 上中央座標を基準に文字表示（表示内容, x, y, フォント）
  canvas.drawCentreString("ブログ", 64, 65, &fonts::lgfxJapanGothicP_28);    // 上中央座標を基準に文字表示（表示内容, x, y, フォント）
  

  // canvas.clear(M5.Lcd.color565(20, 0, 0)); // 背景色
  // canvas.setCursor(0, 10);              // カーソル座標
  // canvas.setFont(&fonts::Font7);        // フォント
  // if (limit_flag == false) {            // カウントリミットフラグがfalseなら
  //   canvas.setTextColor(TFT_YELLOW);    // 文字色（黄）
  // } else {                              // カウントリミットフラグがtrueなら
  //   canvas.setTextColor(TFT_RED);       // 文字色（赤）
  // }
  // canvas.setTextSize(0.9);              // 文字サイズ倍率
  // canvas.printf("%d:%02d", min_cnt, sec_cnt); // カウント値表示（分：秒）

  // canvas.setCursor(95, 27);             // カーソル座標
  // canvas.setTextSize(0.47);             // 文字サイズ倍率
  // canvas.printf(":%02d", msec_cnt);     // カウント値表示（：ミリ秒）
  // canvas.pushSprite(0, 48); // 座標を指定してメモリ描画領域表示実行(x, y)delay(100);
}

void initDisplay(){
  // 液晶初期化
  M5.Lcd.init();                         // 初期化
  M5.Lcd.setTextWrap(false);             // テキストが画面からはみ出した時の折り返し無し
  M5.Lcd.clear(M5.Lcd.color565(0, 0, 0));  // 背景色
  M5.Lcd.setTextColor(TFT_WHITE);        // 文字色

  // 初期画面
  M5.Lcd.fillRect(0, 0, 128, 25, TFT_WHITE);       // タイトルエリア背景
  M5.Lcd.setTextColor(M5.Lcd.color565(20, 20, 20));   // 文字色
  M5.Lcd.drawString("ATOM", 12, 2, &fonts::Font4); // 上中央座標を基準に文字表示（表示内容, x, y）
  M5.Lcd.setTextColor(TFT_RED);                    // 文字色
  M5.Lcd.drawString("S3", 88, 2, &fonts::Font4);   // 上中央座標を基準に文字表示（表示内容, x, y）
  M5.Lcd.fillRect(16, 14, 7, 2, TFT_RED);          // ATOMの「A」の横線用

  M5.Lcd.setTextColor(TFT_CYAN);                   // 文字色
  M5.Lcd.drawCentreString("3分ｶｳﾝﾄﾀﾞｳﾝﾀｲﾏ", 64, 28, &fonts::lgfxJapanGothicP_16); // 上中央座標を基準に文字表示（表示内容, x, y, フォント）
  M5.Lcd.drawFastHLine(0, 46, 128, TFT_WHITE);     // 指定座標から横線(x, y, 長さ, 色)
  M5.Lcd.drawFastHLine(0, 112, 128, TFT_WHITE);    // 指定座標から横線(x, y, 長さ, 色)
  M5.Lcd.setTextColor(TFT_WHITE);                  // 文字色
  M5.Lcd.drawCentreString("Long Press Reset", 64, 113, &fonts::Font2); // 上中央座標を基準に文字表示（表示内容, x, y, フォント）

  // メモリ描画(カウント値表示)領域初期化
  canvas.setColorDepth(16);     // カラーモード設定(16bit)
  canvas.setTextWrap(false);    // テキストが画面からはみ出した時の折り返し無し
  canvas.createSprite(128, 64); // メモリ描画(カウント値表示)領域を設定(X, Y)
}