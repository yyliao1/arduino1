#include <ESP8266WiFi.h>

//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
#include <string>

//ESP8266WebServer server(80); //設定Web server & port 為 80 (http protocol 專用Port
const char* ssid     = "YYap";
const char* password = "1111111111";
IPAddress ip(192, 168, 1, 111); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const char* entity = "ENT01";
const char* serverIP = "192.168.1.100";
String line = "";

void setup() {
  Serial.end();
  Serial.begin(115200);

  Serial.println("Begin");

  pinMode(5, INPUT);

  WiFi.begin(ssid, password);
  //WiFi.config(ip, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // MDNS.begin("esp8266-1"); //啟用Multi DNS, 設定esp8266.local為本機網域及主機名稱
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  Serial.println("Loop");
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(serverIP, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "http://";
  url += serverIP;
  url += "/deepSelect.php?entity=";

  // url += "&value=";
   url += entity;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + serverIP + "\r\n" +
               "Connection: close\r\n\r\n");

  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    line = client.readStringUntil('\r');
  }
  Serial.println(line);
  int yy = line[15] - '0';
  if (yy > 0) {
    ESP.deepSleep(20000000, WAKE_RF_DEFAULT);
  }

  // delay(10000000);
}
