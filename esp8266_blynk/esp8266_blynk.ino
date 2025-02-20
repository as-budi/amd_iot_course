#define BLYNK_TEMPLATE_ID "TMPL6gFT1E1DE"
#define BLYNK_TEMPLATE_NAME "ESP8266IoT"
#define BLYNK_AUTH_TOKEN "iVMwS8p6ymB1lOt1h33SAjKj7-g7tCrl"

//hardWare Specifics
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <AceButton.h>

using namespace ace_button;

const char *ssid = "zenfone9";
const char *password = "zxcvbnm9";

bool fetch_blynk_state = true;

#define VPIN_LED V0

#define LED 2

#define button1 16

int wifiFlag = 0;

bool ledState = LOW;

char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;

ButtonConfig config1;
AceButton aBtn1(&config1);

void button1Handler(AceButton*, uint8_t, uint8_t);

BLYNK_WRITE(VPIN_LED){
  ledState = param.asInt();
  digitalWrite(LED, ledState);
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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(button1, INPUT_PULLUP);

  config1.setEventHandler(button1Handler);

  aBtn1.init(button1);

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

  digitalWrite(LED, ledState);

  if(!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_LED, ledState);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run();

  aBtn1.check();
}

void button1Handler(AceButton*, uint8_t eventType, uint8_t){
  switch(eventType){
    case AceButton::kEventReleased:
    ledState = !ledState;
    digitalWrite(LED, ledState);
    Blynk.virtualWrite(VPIN_LED, ledState);
    break;
  }
}

