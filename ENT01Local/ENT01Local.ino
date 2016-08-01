#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
//#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <FS.h>
//#define USE_SERIAL Serial
ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "YYap";
const char* password = "1111111111";
const char* host = "lamp01";
IPAddress ip(192, 168, 1, 111); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const char* entity = "ENT01";
const char* serverIP = "192.168.1.168";
String line = "";



#define LEDPIN D0

#define IN false
#define OUT true
#define HI true
#define LO false
uint16_t g_gpiooutputmask = 0;

ESP8266WebServer server(80);

File fsUploadFile;

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  Serial.println("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  Serial.println("handleFileCreate: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
 Serial.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  output += "]";
  server.send(200, "text/json", output);
}

//***********Convert charactor to Integer ************
int32_t my_atoi( const char * in )
{
  int positive = 1; //1 if negative.
  int hit = 0;
  int val = 0;
  while ( *in && hit < 11  )
  {
    if ( *in == '-' )
    {
      if ( positive == -1 ) return val * positive;
      positive = -1;
    } else if ( *in >= '0' && *in <= '9' )
    {
      val *= 10;
      val += *in - '0';
      hit++;
    } else if (!hit && ( *in == ' ' || *in == '\t' ) )
    {
      //okay
    } else
    {
      //bad.
      return val * positive;
    }
    in++;
  }
  return val * positive;
}
// *********** end of convert***********

//*********** WebSocket & GPIO Control **************
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  String text = String((char *) &payload[0]);
  char * textC = (char *) &payload[0];
  String rssi;
  String temp;
  int nr;
  int on;
  uint32_t rmask;
  int i;
  char b[10];   //declaring character array
  String str;  //declaring string
  static const uint32_t AFMapper[16] = {
    0, PERIPHS_IO_MUX_U0TXD_U, 0, PERIPHS_IO_MUX_U0RXD_U,
    0, 0, 1, 1,
    1, 1, 1, 1,
    PERIPHS_IO_MUX_MTDI_U, PERIPHS_IO_MUX_MTCK_U, PERIPHS_IO_MUX_MTMS_U, PERIPHS_IO_MUX_MTDO_U
  };

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");

      switch (payload[0]) {
        case 'w': case 'W':  // Request RSSI wx
          rssi = String(WiFi.RSSI());
          // Serial.printf("[%u] Got message: %s\n", num, payload);
          webSocket.sendTXT(0, rssi);
          break;

        case 'C': // not sure... CVR
          Serial.printf("[%u] Got message: %s\n", num, payload);
          webSocket.sendTXT(0, "");
          break;

        case 'e': case 'E':   //Echo
          webSocket.sendTXT(0, text);
          break;
        case 'G': // GPIO
          {
            Serial.printf("[%u] Got message: %s\n", num, payload);
            int pin = payload[1] - '0';
            int value;
            value = payload[2] - '0';
            pinMode(pin, OUTPUT);
            digitalWrite(pin, value);
            break;
            webSocket.sendTXT(0, b);
          }
          break;

        default:
          webSocket.sendTXT(0, "**** UNDEFINED ****");
          Serial.printf("[%u] Got UNDEFINED message: %s\n", num, payload);
          break;
      }
      break;
  }
}
//************ End of WebSocket *****************

void setup() {
  Serial.begin(115200);
  Serial.print("\n");

  digitalWrite(LED_BUILTIN, HIGH);
  //pinMode(LEDPIN, OUTPUT);

  // Connect tp Wifi
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  MDNS.begin(host);
 
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

//SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {

  server.handleClient();
  webSocket.loop();

//***********Call Deep Sleep DB and Start Deep Sleep *********** 
  String url = "http://";
  url += serverIP;
  url += "/deepSelect.php?entity=";
  url += entity;

  // if((WiFiMulti.run() == WL_CONNECTED)) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      line = http.getString();
    //  Serial.println(line);
      int yy = line[14] - '0';
      if (yy > 0) {
        ESP.deepSleep(20000000, WAKE_RF_DEFAULT);
      }
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  //}
  //**********end Check DB for deep sleep *********************

}
