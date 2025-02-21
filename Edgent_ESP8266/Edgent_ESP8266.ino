/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
 *************************************************************
  Blynk.Edgent implements:
  - Blynk.Inject - Dynamic WiFi credentials provisioning
  - Blynk.Air    - Over The Air firmware updates
  - Device state indication using a physical LED
  - Credentials reset using a physical Button
 *************************************************************/

/* Fill in information from your Blynk Template here */
/* Read more: https://bit.ly/BlynkInject */
#define BLYNK_TEMPLATE_ID "TMPL6C6nrDFPr"
#define BLYNK_TEMPLATE_NAME "ESP8266"

#define BLYNK_FIRMWARE_VERSION        "0.1.2"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
//#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"
#include <AceButton.h>

using namespace ace_button;

bool fetch_blynk_state = true;

#define VPIN_LED V0

#define LED 2

#define button1 16

int wifiFlag = 0;

bool ledState = LOW;

// char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;

ButtonConfig config1;
AceButton aBtn1(&config1);

void button1Handler(AceButton*, uint8_t, uint8_t);

BLYNK_WRITE(VPIN_LED){
  ledState = param.asInt();
  digitalWrite(LED, !ledState);
}


void checkBlynkStatus(){
  bool isConnected = Blynk.connected();
  if(isConnected == false){
    wifiFlag = 1;
    Serial.println("Blynk tidak terhubung!");
  }else{
    wifiFlag = 0;
    if(!fetch_blynk_state){
      Blynk.virtualWrite(VPIN_LED, ledState);
    }
    Serial.println("Blynk terhubung");
  }
}

BLYNK_CONNECTED(){
  if(fetch_blynk_state){
    Blynk.syncVirtual(VPIN_LED);
  }
}


void setup()
{
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  pinMode(LED, OUTPUT);
  pinMode(button1, INPUT_PULLUP);

  config1.setEventHandler(button1Handler);

  aBtn1.init(button1);

  digitalWrite(LED, !ledState);

  if(!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_LED, ledState);
  }
}

void loop() {
  BlynkEdgent.run();
  Blynk.run();
  timer.run();

  aBtn1.check();

  // digitalWrite(LED, LOW);
  // delay(1000);
  // digitalWrite(LED, HIGH);
  // delay(1000);
}

void button1Handler(AceButton*, uint8_t eventType, uint8_t){
  switch(eventType){
    case AceButton::kEventReleased:
    ledState = !ledState;
    digitalWrite(LED, !ledState);
    Blynk.virtualWrite(VPIN_LED, ledState);
    break;
  }
}
