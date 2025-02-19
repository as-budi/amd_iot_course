#define BLYNK_TEMPLATE_ID "TMPL6-negEDLA"
#define BLYNK_TEMPLATE_NAME "ESP32IoT"
#define BLYNK_AUTH_TOKEN "nryVXJQdPirAl4PBlzGFSL6KNwcUU5Nx"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <AceButton.h>

using namespace ace_button;

const char *ssid = "ASB";
const char *password = "zxcvbnm9";

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

char auth[] = BLYNK_AUTH_TOKEN;
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.println("Sedang proses menghubungkan ke WiFI ...");
    delay(500);
  }
  Serial.print("Berhasil terhubung ke WiFi dengan IP Address: ");
  Serial.println(WiFi.localIP());

  timer.setInterval(2000L, checkBlynkStatus);
  Blynk.config(auth);
  delay(1000);

  digitalWrite(LED_R, rState);
  digitalWrite(LED_G, gState);

  if(!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_R, rState);
    Blynk.virtualWrite(VPIN_G, gState);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run();

  aBtn1.check();
  aBtn2.check();
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
