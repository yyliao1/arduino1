#include <ESP8266WiFi.h>

//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>

//ESP8266WebServer server(80); //設定Web server & port 為 80 (http protocol 專用Port
const char* ssid     = "YYap";
const char* password = "1111111111";
IPAddress ip(192, 168, 1, 111); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const char* host = "192.168.1.100";

void setup() {

  
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
  Serial.println("data");
    Serial.println(digitalRead(5));

 Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
 // client.connect('http://localhost/test3.php?event=EVENT&entity=ENTITY&measure=100.4',80);
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
// We now create a URI for the request
//http://localhost/test3.php?event=EVENT&entity=ENTITY&measure=100.4
 String url = "http://";
        url += host;
        url +="/test3.php?event=CHANGE&entity=SW01&measure=";
 
 // url += "&value=";
  url += digitalRead(5);
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
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
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  
    
 ESP.deepSleep(20000000,WAKE_RF_DEFAULT);
 Serial.println("Loop2");
// delay(10000000);
}
