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
#define BLYNK_TEMPLATE_ID "TMPL6-negEDLA"
#define BLYNK_TEMPLATE_NAME "ESP32IoT"

#define BLYNK_FIRMWARE_VERSION        "0.1.2"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_ESP32_DEV_MODULE
//#define USE_ESP32C3_DEV_MODULE
//#define USE_ESP32S2_DEV_KIT
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7
//#define USE_TTGO_T_OI

#include <WiFi.h>
#include "BlynkEdgent.h"
#include <AceButton.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN 12

Adafruit_BMP085 bmp;

using namespace ace_button;

bool fetch_blynk_state = true;

#define VPIN_R V0
#define VPIN_G V1

#define LED_R 13
#define LED_G 14

#define button1 16
#define button2 17

#define wifiLed 2
int wifiFlag = 0;

bool rState = LOW;
bool gState = LOW;

BlynkTimer timer;

ButtonConfig config1;
AceButton aBtn1(&config1);
ButtonConfig config2;
AceButton aBtn2(&config2);

void button1Handler(AceButton*, uint8_t, uint8_t);
void button2Handler(AceButton*, uint8_t, uint8_t);

BLYNK_WRITE(VPIN_R){
  rState = param.asInt();
  digitalWrite(LED_R, rState);
}

BLYNK_WRITE(VPIN_G){
  gState = param.asInt();
  digitalWrite(LED_G, gState);
}

void checkBlynkStatus(){
  bool isConnected = Blynk.connected();
  if(isConnected == false){
    wifiFlag = 1;
    Serial.println("Blynk tidak terhubung!");
    digitalWrite(wifiLed, LOW);
  }else{
    wifiFlag = 0;
    if(!fetch_blynk_state){
      Blynk.virtualWrite(VPIN_R, rState);
      Blynk.virtualWrite(VPIN_G, gState);
    }
    digitalWrite(wifiLed, HIGH);
    Serial.println("Blynk terhubung");
  }
}

BLYNK_CONNECTED(){
  if(fetch_blynk_state){
    Blynk.syncVirtual(VPIN_R);
    Blynk.syncVirtual(VPIN_R);
  }
}

const long interval = 5000;
unsigned long lastCheck = 0;
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  pinMode(wifiLed, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);

  aBtn1.init(button1);
  aBtn2.init(button2);

  digitalWrite(wifiLed, LOW);

  timer.setInterval(2000L, checkBlynkStatus);
  delay(1000);

  digitalWrite(LED_R, rState);
  digitalWrite(LED_G, gState);

  if(!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_R, rState);
    Blynk.virtualWrite(VPIN_G, gState);
  }

  dht.begin();

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1) {}
  }
}

void loop() {
  BlynkEdgent.run();

  Blynk.run();
  timer.run();

  aBtn1.check();
  aBtn2.check();

  if((millis() - lastCheck) >= interval){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int p = bmp.readPressure();

    Blynk.virtualWrite(V2, t);
    Blynk.virtualWrite(V3, h);
    Blynk.virtualWrite(V4, p);

    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Pressure = ");
    Serial.println(p);

    lastCheck = millis();
  }
}

void button1Handler(AceButton*, uint8_t eventType, uint8_t){
  switch(eventType){
    case AceButton::kEventReleased:
    rState = !rState;
    digitalWrite(LED_R, rState);
    Blynk.virtualWrite(VPIN_R, rState);
    break;
  }
}

void button2Handler(AceButton*, uint8_t eventType, uint8_t){
  switch(eventType){
    case AceButton::kEventReleased:
    gState = !gState;
    digitalWrite(LED_G, gState);
    Blynk.virtualWrite(VPIN_G, gState);
    break;
  }
}

