#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "YYap";
const char* password = "1111111111";

ESP8266WebServer server(80);

const int led = 13;
const int relay01 = 5;


void handleRoot() {
  //digitalWrite(led, 1);
  //server.send(200, "text/plain", "hello from esp8266!");
  //digitalWrite(led, 0);
  String s = "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>";
   s += " <meta name=\"viewport\" content=\"user-scalable=no, initial-scale=1, maximum-scale=1, minimum-scale=1, width=device-width\">";
   s += "<p><a href=\"\\on\"><button>On</button></a>";
   s += "<p><a href=\"\\off\"><button>Off</button></a>";
  // s += "<form>";
 // s += "<button type=submit formmethod=post formaction=\"http://esp8266.local/on\">ON</button>";
 //s += " <button type=submit formmethod=post formaction=\"http://esp8266.local/off\">OFF</button>";
//s += "</form>";
  s += "</html>\n";
  server.send(200, "text/html", s);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  pinMode(relay01,OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

 server.on("/on", [](){
  digitalWrite(relay01, 1);
    String s = "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>";
   s += " <meta name=\"viewport\" content=\"user-scalable=no, initial-scale=1, maximum-scale=1, minimum-scale=1, width=device-width\">";
   s += "<p><a href=\"\\on\"><button>On</button></a>";
   s += "<p><a href=\"\\off\"><button>Off</button></a>";
  s += "</html>\n";
  server.send(200, "text/html", s);
  });

 server.on("/off", [](){
  digitalWrite(relay01, 0);
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
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
