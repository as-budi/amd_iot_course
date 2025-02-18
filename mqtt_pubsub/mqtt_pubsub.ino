#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>

const char *AP_SSID = "ASB";
const char *AP_PWD = "zxcvbnm9";

const char *mqtt_broker = "broker.emqx.io";
const char *topic_sub = "amd_iot/led";
const char *topic_pub = "amd_iot/sensor";
const int mqtt_port = 1883;
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";

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
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

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

  if(mqtt_client.subscribe(topic_sub)){
    Serial.printf("ESP32 subscribed to topic: %s", topic_sub);
  }else{
    Serial.println("Failed to subscribe");
  }
  JSONVar myData;
  myData["name"] = "Temperature Sensor";
  myData["data"] = (int) 25;

  String body = JSON.stringify(myData);
  mqtt_client.publish(topic_pub, body.c_str());
}

void loop() {
  // put your main code here, to run repeatedly:
  mqtt_client.loop();
}
