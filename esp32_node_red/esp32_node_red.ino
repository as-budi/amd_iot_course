#include <WiFi.h>
#include <PubSubClient.h>
#include <AceButton.h>
#include "DHT.h"

using namespace ace_button;

#define DHTTYPE DHT11
#define DHTPIN 12
DHT dht(DHTPIN, DHTTYPE);

const char *AP_SSID = "ASB";
const char *AP_PWD = "zxcvbnm9";

const char *mqtt_broker = "54.169.36.133";
const char *sub_led_merah = "amd/led_merah";
const char *sub_led_hijau = "amd/led_hijau";
const char *pub_led_merah = "amd/led_merah";
const char *pub_led_hijau = "amd/led_hijau";
const char *pub_temperature = "amd/temperature";
const char *pub_humidity = "amd/humidity";
const int mqtt_port = 1883;
const char *mqtt_username = "as-budi";
const char *mqtt_password = "satu+1samadengan2";

#define LED_R 13
#define LED_G 14

#define button1 16
#define button2 17

bool rState = LOW;
bool gState = LOW;

ButtonConfig config1;
AceButton aBtn1(&config1);
ButtonConfig config2;
AceButton aBtn2(&config2);

void button1Handler(AceButton*, uint8_t, uint8_t);
void button2Handler(AceButton*, uint8_t, uint8_t);

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void ConnectToMqttBroker(){
  while(!mqtt_client.connected()){
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.println("Connecting ...");
    if(mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)){
      Serial.println("Connected to Broker");
    }else{
      Serial.println("Failed to Connect");
      Serial.println(mqtt_client.state());
      delay(2000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length){
  String sMsg;
  Serial.print("Message received in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for(int i = 0; i < length; i++){
    sMsg += (char) payload[i];
  }
  Serial.println(sMsg);

  if (strcmp(topic, "amd/led_hijau")==0){
    if (sMsg == "1"){
      gState = HIGH;
      digitalWrite(LED_G, gState);
    }else {
      gState = LOW;
      digitalWrite(LED_G, gState);
    }
  }
  if (strcmp(topic, "amd/led_merah")==0){
    if (sMsg == "1"){
      rState = HIGH;
      digitalWrite(LED_R, rState);
    }else {
      rState = LOW;
      digitalWrite(LED_R, rState);
    }
  }
}

const long interval = 5000;
unsigned long lastCheck = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);

  aBtn1.init(button1);
  aBtn2.init(button2);

  WiFi.begin(AP_SSID, AP_PWD); //menghubungkan ke akses poin
  while(WiFi.status() != WL_CONNECTED){
    Serial.println("Sedang proses menghubungkan ke WiFI ...");
    delay(500);
  }
  Serial.print("Berhasil terhubung ke WiFi dengan IP Address: ");
  Serial.println(WiFi.localIP());

  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(callback);
  ConnectToMqttBroker();

  if(mqtt_client.subscribe(sub_led_merah)){
    Serial.printf("ESP32 subscribed to topic: %s", sub_led_merah);
  }else{
    Serial.printf("Failed to subscribe to topic: %s", sub_led_merah);
  }

  if(mqtt_client.subscribe(sub_led_hijau)){
    Serial.printf("ESP32 subscribed to topic: %s", sub_led_hijau);
  }else{
    Serial.printf("Failed to subscribe to topic: %s", sub_led_hijau);
  }

  dht.begin();

  // mqtt_client.publish(topic_pub, body.c_str());
}

void loop() {
  // put your main code here, to run repeatedly:

  mqtt_client.loop();

  aBtn1.check();
  aBtn2.check();

  if((millis() - lastCheck) >= interval){
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.println(h);

    String sT = String(t);
    String sH = String(h);

    mqtt_client.publish(pub_temperature, sT.c_str());
    mqtt_client.publish(pub_humidity, sH.c_str());

    lastCheck = millis();
  }
}

void button1Handler(AceButton*, uint8_t eventType, uint8_t){
  switch(eventType){
    case AceButton::kEventReleased:
    Serial.println("pressed");
    rState = !rState;
    digitalWrite(LED_R, rState);
    String sRed;
    if(rState == HIGH){
      sRed = "1";
    }else{
      sRed = "0";
    }
    mqtt_client.publish(pub_led_merah, sRed.c_str());
    Serial.println("published led_merah");
    break;
  }
}

void button2Handler(AceButton*, uint8_t eventType, uint8_t){
  switch(eventType){
    case AceButton::kEventReleased:
    Serial.println("pressed");
    gState = !gState;
    digitalWrite(LED_G, gState);
    String sGreen;
    if(gState == HIGH){
      sGreen = "1";
    }else{
      sGreen = "0";
    }
    mqtt_client.publish(pub_led_hijau, sGreen.c_str());
    Serial.println("published led_hijau");
    break;
  }
}
