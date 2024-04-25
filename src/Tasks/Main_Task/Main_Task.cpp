#include "Main_Task.h"

void main_task(void *pvParameters){
  // 3つのモードを順に回す
  while (1){
    // Serial Mode
    while (digitalRead(DISPLAY_BUTTON) == LOW){
      GET_SERAL_DATA();
      SNED_SERIAL();
      SEND_ANOTHER_TASK();
    }
    while (digitalRead(DISPLAY_BUTTON) == HIGH);

    // Bluetooth Mode
    while (digitalRead(DISPLAY_BUTTON) == LOW){
      GET_SERAL_DATA();
      SEND_BLUETOOTH();
      SEND_ANOTHER_TASK();
    }
    while (digitalRead(DISPLAY_BUTTON) == HIGH);

    // All Mode
    while (digitalRead(DISPLAY_BUTTON) == LOW){
      GET_SERAL_DATA();
      SNED_SERIAL();
      SEND_BLUETOOTH();
      SEND_ANOTHER_TASK();
    }
    while (digitalRead(DISPLAY_BUTTON) == HIGH);
  }
}

static void GET_SERAL_DATA(){

}

static void SNED_SERIAL(){

}

static void SEND_BLUETOOTH(){

}

static void SEND_ANOTHER_TASK(){

}