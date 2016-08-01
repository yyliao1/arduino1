#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "YYap"; //無線網路名稱
const char* password = "1111111111"; //無線網路密碼
const char* host = "tv";

ESP8266WebServer server(80); //設定Web server & port 為 80 (http protocol 專用Port

//const int relay01 = 5; // Relay = GPIO05

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
  int gpioNo=5;
  int modeType=0;
  int highLow=0;
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
     if ( server.argName(i) == "GPIO") {
       gpioNo = server.arg(i).toInt();
    }
    if ( server.argName(i) == "MODE") {
      if (server.arg(i)="INPUT"){
        modeType=0;
      }
     if (server.arg(i)="OUTPUT"){
        modeType=1;
      }
    }
    if ( server.argName(i) == "STA") {
       highLow = server.arg(i).toInt();     
    }
  }
  pinMode(gpioNo,modeType);
  digitalWrite(gpioNo,highLow);
  server.send(404, "text/html", "<BODY onLoad=\"setTimeout('exit()', 2000)\"></BODY>");
}

void setup(void) {

 // pinMode(relay01, OUTPUT); //設定REPLAY PIN (GPIO05) 為輸出格式
  WiFi.begin(ssid, password); //啟動Wifi
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
  }
  
  MDNS.begin(host); //啟用Multi DNS, 設定esp8266.local為本機網域及主機名稱
   
  server.on("/", handleRoot); // Home page call function handleRoot

  server.onNotFound(handleNotFound);
  server.begin();
}

void loop(void) {
  server.handleClient();
}
