#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "YYap"; //無線網路名稱
const char* password = "1111111111"; //無線網路密碼

ESP8266WebServer server(80); //設定Web server & port 為 80 (http protocol 專用Port

const int relay01 = 5; // Relay = GPIO05

void handleRoot() { //首頁
  
  String s = "<!DOCTYPE HTML>\r\n<html>\r\n"; // HTML5
  s += "<head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"; //Charset
  s += " <meta name=\"viewport\" content=\"user-scalable=no, initial-scale=1, maximum-scale=1, minimum-scale=1, width=device-width\">"; //手機自動調大小
  s += "<p><a href=\"\\on\"><button>On</button></a>"; //Button On
  s += "<p><a href=\"\\off\"><button>Off</button></a>"; //Button off
  s += "</html>\n";
  server.send(200, "text/html", s); //以HTML格式傳送到 ESP8266 SERVER PORT
}

void handleNotFound() { 
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {

  pinMode(relay01, OUTPUT); //設定REPLAY PIN (GPIO05) 為輸出格式
  WiFi.begin(ssid, password); //啟動Wifi
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
  }
  
  MDNS.begin("esp8266"); //啟用Multi DNS, 設定esp8266.local為本機網域及主機名稱
   
  server.on("/", handleRoot); // Home page call function handleRoot

  server.on("/on", []() {
    digitalWrite(relay01, 1);  // Set Relay On
    String s = "<!DOCTYPE HTML>\r\n<html>\r\n";
    s += "<head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>";
    s += " <meta name=\"viewport\" content=\"user-scalable=no, initial-scale=1, maximum-scale=1, minimum-scale=1, width=device-width\">";
    s += "<p><a href=\"\\on\"><button>On</button></a>";
    s += "<p><a href=\"\\off\"><button>Off</button></a>";
    s += "</html>\n";
    server.send(200, "text/html", s);
  });

  server.on("/off", []() {
    digitalWrite(relay01, 0);  // Set Relay Off
    String s = "<!DOCTYPE HTML>\r\n<html>\r\n";
    s += "<head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>";
    s += " <meta name=\"viewport\" content=\"user-scalable=no, initial-scale=1, maximum-scale=1, minimum-scale=1, width=device-width\">";
    s += "<p><a href=\"\\on\"><button>On</button></a>";
    s += "<p><a href=\"\\off\"><button>Off</button></a>";
    s += "</html>\n";
    server.send(200, "text/html", s);
  });
  
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop(void) {
  server.handleClient();
}
