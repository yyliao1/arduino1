/*
 This program is for GPIO 5 input signal, for ESP8266-07 
 1. Connect Wifi failed will deep sleep for 30 seconds
 2. Connect DB failed will deep sleep for 30 seconds
 3. call php server to insert the state of gpio5 to database
 */
#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "YYap";
const char* password = "1111111111";
const char* host = "ent03";
IPAddress ip(192, 168, 1, 113); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const char* entity = "ENT03";
const char* serverIP = "192.168.1.100";
String line = "";

void setup(void) {
  Serial.begin(115200);
  //digitalWrite(LED_BUILTIN, LOW);
  pinMode(5, INPUT); //GPIO5 INPUT
  //WIFI INIT
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
  }

  // Wifi connection check otherwise deep sleep
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  /*  i ++;
    if (i > 5) {
      ESP.deepSleep(3000000, WAKE_RF_DEFAULT);
    }
    */
    Serial.print(".");
    Serial.print(i);
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(void) {

  //***********Call Deep Sleep DB and Start Deep Sleep ***********
  String url = "http://";
  url += serverIP;
  url += "/deepSelect.php?entity=";
  url += entity;

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      line = http.getString();
      //  Serial.println(line);
      int yy = line[14] - '0';
      if (yy > 0) {
        ESP.deepSleep(300000000, WAKE_RF_DEFAULT);
      }
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    ESP.deepSleep(300000000, WAKE_RF_DEFAULT);
  }
  http.end();
  //**********end Check DB for deep sleep *********************

  //********* Read GPIO5 and Call PHP update database
  url = "http://";
  url += serverIP;
  url += "/test3.php?event=CHANGE&entity="; 
  url += entity;
  url += "&measure=";
  url += digitalRead(5);
  http.begin(url);
  http.GET();
  Serial.println(url);
  http.end();
  ESP.deepSleep(30000000, WAKE_RF_DEFAULT);
  //********************************
}
