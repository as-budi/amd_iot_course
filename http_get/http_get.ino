#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char *AP_SSID = "ASB";
const char *AP_PWD = "zxcvbnm9";

String apiUrl = "https://api.restful-api.dev/objects";

void parsing(JSONVar myObject){
  if(JSON.typeof(myObject) == "undefine"){
    Serial.println("Parsing gagal, karena type data tidak sesuai!");
    return;
  }else{
    if(myObject.hasOwnProperty("name")){
      Serial.print("\"name\": ");
      Serial.println((const char*) myObject["name"]);
    }
  }
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

  HTTPClient http; //membuat object http dari kelas HTTPCLient
  String apiPath = apiUrl + "?id=2";
  http.begin(apiPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0){
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);

    payload.remove(payload.length()-1, 1);
    payload.remove(0, 1);

    Serial.print("Payload setelah cleaning: ");
    Serial.println(payload);

    JSONVar myObject = JSON.parse(payload);
    parsing(myObject);    
  }else{
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
