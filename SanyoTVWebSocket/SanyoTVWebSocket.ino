/*
   IRremoteESP8266: IRsendGCDemo - demonstrates sending Global Cache-formatted IR codes with IRsend
   An IR LED must be connected to ESP8266 pin 0.
   Version 0.1 30 March, 2016
   Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009, Copyright 2009 Ken Shirriff, http://arcfn.com
*/

//********** Inclue lib ******************************
#include <IRremoteESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ESP8266HTTPUpdateServer.h>

//********** Network Setting *******************
const char* ssid = "YYap";
const char* password = "1111111111";
const char* host = "ent06";
IPAddress ip(192, 168, 1, 116); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const char* entity = "ENT06";
const char* serverIP = "192.168.1.168";
String line = ""; // Deep sleep

//*********** Alias Name ************************
WiFiClient client;
HTTPClient http;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WebSocketsServer webSocket = WebSocketsServer(81);
File fsUploadFile;

//************ ESP8266 Pin Definition **********************
IRsend irsend(5); //an IR emitter led is connected to GPIO pin 5


// Codes are in Global Cache format less the emitter ID and request ID. These codes can be found in GC's Control Tower database.
unsigned int  Power[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 7B3C707B
unsigned int  TVAV[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600}; // UNKNOWN 1621E97F
unsigned int  sleep[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 7888C107
unsigned int  VideoPause[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN E946ACDF
unsigned int  widescreen[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 500, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 1F0CE6BF
unsigned int  MTS[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 500, 1750, 600, 600, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 63C6429F
unsigned int  display1[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 1EDB6903
unsigned int  programGuide[67] = {9000, 4500, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 600, 550, 1750, 600}; // UNKNOWN 3B996343

unsigned int  audioBrodcast[67] = {9000, 4550, 550, 650, 600, 600, 500, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 650, 1650, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 650, 1650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 600, 500, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN 2E890E3F
unsigned int  key1[67] = {9000, 4550, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600}; // UNKNOWN 1278125F
unsigned int  key2[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 99E5473F

unsigned int  key3[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 8ACA81C3

unsigned int  key4[67] = {9000, 4550, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600}; // UNKNOWN D9A8E7DF
unsigned int  key5[67] = {9000, 4550, 550, 650, 550, 600, 600, 600, 550, 1750, 600, 1750, 550, 1700, 550, 650, 550, 600, 600, 1750, 550, 1700, 550, 1750, 600, 600, 550, 650, 550, 600, 600, 1750, 550, 1700, 550, 1750, 600, 600, 550, 1750, 600, 600, 550, 650, 550, 600, 600, 600, 550, 650, 550, 600, 600, 1750, 550, 600, 600, 1750, 550, 1700, 550, 1750, 600, 1700, 600, 1700, 550}; // UNKNOWN 33563FE3
unsigned int  key6[67] = {9000, 4550, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 600, 500, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600}; // UNKNOWN 8756317B
unsigned int  key7[67] = {9000, 4550, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600}; // UNKNOWN 783B6BFF
unsigned int  key8[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 13E7103F
unsigned int  key9[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 500, 1750, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 6D946843

unsigned int  hundred[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 2B8D989F

unsigned int  key0[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 500, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 650, 1650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 1750, 650, 1650, 550, 1750, 500, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 8203C063
unsigned int  channelBack[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN 764D1843

unsigned int  chUp[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 600, 500, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN B24DC423
unsigned int  chDown[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 600, 500, 1750, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN 5CFD12DF

unsigned int  volUp[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 500, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 600, 550, 1750, 650, 550, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN CC88B29F
unsigned int  volDown[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 500, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 650, 1650, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 500, 650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN BD6DED23

unsigned int  directory[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550}; // UNKNOWN E3AC0943
unsigned int  chTable[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 600, 500, 1750, 600, 600, 550, 1750, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN C688A6DF

unsigned int  mute[67] = {9000, 4550, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600}; // UNKNOWN C55F8EBB
unsigned int  exit1[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN 41182403

unsigned int  up[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN DA687EE3
unsigned int  down[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 600, 500, 1750, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN A8C2F87F
unsigned int  left[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 600, 500, 650, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN 5FACF5DF
unsigned int  right[67] = {9000, 4550, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN B95A4DE3

unsigned int  ok[67] = {9000, 4600, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 1750, 600, 600, 550, 1750, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 1750, 550}; // UNKNOWN 6ADCD0DF

unsigned int  videoMode[67] = {9000, 4550, 550, 650, 700, 500, 550, 650, 550, 1750, 550, 1750, 650, 1650, 550, 650, 650, 550, 500, 1750, 650, 1650, 550, 1750, 550, 650, 550, 650, 650, 550, 550, 1750, 650, 1650, 550, 650, 650, 1650, 550, 1750, 550, 650, 550, 650, 650, 550, 550, 1750, 650, 550, 500, 1750, 650, 550, 550, 650, 550, 1750, 550, 1750, 650, 1650, 550, 650, 650, 1650, 550}; // UNKNOWN A6300D4B
unsigned int  audioMode[67] = {9000, 4550, 550, 650, 600, 600, 500, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 1750, 500, 1750, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN BA8BA8BB

unsigned int  red[67] = {9000, 4550, 550, 650, 600, 600, 500, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 500, 1750, 600, 1700, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN 3CF5B8E3
unsigned int  green[67] = {9000, 4550, 550, 650, 600, 600, 500, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 650, 550, 1750, 550, 1750, 600, 600, 500, 1750, 600, 600, 500, 1750, 650, 1650, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN E2EB93AC
unsigned int  yellow[67] = {9000, 4550, 550, 650, 600, 600, 500, 650, 550, 1750, 550, 1750, 600, 1700, 550, 650, 600, 600, 550, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 600, 500, 1750, 600, 1700, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 650, 600, 1700, 550, 1750, 500, 650, 550, 650, 600, 1700, 550, 650, 600, 1700, 550}; // UNKNOWN C1195C7B
unsigned int  blue[67] = {9000, 4550, 550, 650, 550, 650, 700, 500, 500, 1750, 650, 1650, 550, 1750, 550, 650, 550, 650, 650, 1650, 550, 1750, 550, 1750, 650, 550, 550, 650, 550, 650, 650, 1650, 550, 1750, 550, 1750, 650, 550, 550, 1750, 650, 1650, 550, 1750, 550, 650, 550, 1750, 550, 650, 550, 650, 650, 1650, 550, 650, 650, 550, 500, 650, 550, 1750, 550, 650, 550, 1750, 550}; // UNKNOWN B79191C


//******************Function  *****************************************
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
        case 'k': case 'K':   //Echo
          for (int i = 0; i < 3; i++) {
            irsend.sendRaw(Power, 67, 38);
            delay(100);
          }
          webSocket.sendTXT(0, text);
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
//************ End of WebSocket ****************

void handleRoot() {
  // Prepare the response. Start with the common header:
  String s = "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>";
  s += " <meta name=\"viewport\" content=\"user-scalable=no, initial-scale=1, maximum-scale=1, minimum-scale=1, width=device-width\">";
  s += "<img src=\"http://yyliao1.github.io/image/TVRC1.jpg\" width=\"400\" height=\"360\" usemap=\"#TestMap\">";
  s += "<map name=\"TestMap\">";
  s += "<area shape=\"rect\" coords=\" 120 , 0 , 170 , 25\" href=\"\\TVPower\">";
  s += "<area shape=\"rect\" coords=\" 13 , 33 , 47 , 60\" href=\"\\TVAV\">";
  s += "<area shape=\"rect\" coords=\" 58 , 33 , 93 , 60\" href=\"\\TVSleep\">";
  s += "<area shape=\"rect\" coords=\" 10 , 104 , 60 , 138\" href=\"\\key1\">";
  s += "<area shape\"rect\" coords=\" 70 , 104 , 120 , 138\" href=\"\\key2\">";
  s += "<area shape\"rect\" coords=\" 140 , 104 , 180 , 138\" href=\"\\key3\">";
  s += "<area shape\"rect\" coords=\" 10 , 144 , 60 , 180\" href=\"\\key4\">";
  s += "<area shape\"rect\" coords=\" 70 , 144 , 120 , 180\" href=\"\\key5\">";
  s += "<area shape\"rect\" coords=\" 140 , 144 , 180 , 180\" href=\"\\key6\">";
  s += "<area shape\"rect\" coords=\" 10 , 190 , 60 , 220\" href=\"\\key7\">";
  s += "<area shape\"rect\" coords=\" 70 , 190 , 120 , 220\" href=\"\\key8\">";
  s += "<area shape\"rect\" coords=\" 140 , 190 , 180 , 220\" href=\"\\key9\">";
  s += "<area shape\"rect\" coords=\" 70 , 229 , 120 , 260\" href=\"\\key0\">";

  s += "<area shape\"rect\" coords=\" 263 , 59 , 336 , 86\" href=\"\\keyUp\">";
  s += "<area shape\"rect\" coords=\" 263 , 165 , 336 , 189\" href=\"\\keyDown\">";
  s += "<area shape\"rect\" coords=\" 225 , 86 , 261 , 160\" href=\"\\keyLeft\">";
  s += "<area shape\"rect\" coords=\" 339 , 90 , 367 , 160\" href=\"\\keyRight\">";
  s += "<area shape\"rect\" coords=\" 274 , 98 , 325 , 145\" href=\"\\keyOK\">";

  s += "<area shape\"rect\" coords=\" 28 , 274 , 65 , 314\" href=\"\\keyChUp\">";
  s += "<area shape\"rect\" coords=\" 28 , 316 , 67 , 360\" href=\"\\keyChDown\">";

  s += "</map>";
  s += "<style>td{font-size:xx-large;}</style>";

  s += "</html>\n";
  server.send(200, "text/html", s);
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
  //  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
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
    //DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    //DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  //Serial.println("handleFileDelete: " + path);
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
  //Serial.println("handleFileCreate: " + path);
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
  //Serial.println("handleFileList: " + path);
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
//***********************Setup*************************
void setup()
{
  Serial.begin(115200);
  irsend.begin();

  //********************File System**************************
  SPIFFS.begin();
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
  }
  //*****************************************************
  // **************Wifi ************************
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  /* while (WiFi.waitForConnectResult() != WL_CONNECTED) {
     WiFi.begin(ssid, password);
     WiFi.config(ip, gateway, subnet);
     Serial.println("WiFi failed, retrying.");
    }*/
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  server.begin();
  httpUpdater.setup(&server);

  MDNS.begin(host);
  MDNS.addService("http", "tcp", 80);

  // Wait for connection
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    i ++;
    if (i > 5) {
      ESP.deepSleep(30000000, WAKE_RF_DEFAULT); //Deep Sleep
    }
  }
  //***************Web server ***********************************
  server.on("/TV", handleRoot);
  server.on("/", HTTP_GET, []() {
    if (!handleFileRead("/index.htm")) server.send(404, "text/plain", "FileNotFound");
  });
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

  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  server.on("/TVPower", []() {
    //  server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(Power, 67, 38);
      delay(50);
    }
    handleRoot();
  });

  server.on("/TVAV", []() {
    //  server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(TVAV, 99, 38);
    }
    handleRoot();
  });

  server.on("/TVSleep", []() {
    //  server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(sleep, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVVideoPause", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(VideoPause, 67, 38);
    }
    handleRoot();
  });
  server.on("/TVWideScreen", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(widescreen, 67, 38);
    }
    handleRoot();
  });
  server.on("/TVMTS", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(MTS, 67, 38);
    }
    handleRoot();
  });
  server.on("/TVDisplay", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(display1, 67, 38);
    }
    handleRoot();
  });
  server.on("/TVProgramGuide", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(programGuide, 67, 38);
    }
    handleRoot();
  });
  server.on("/TVAudioBrodcast", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(audioBrodcast, 67, 38);
    }
    handleRoot();
  });
  server.on("/key1", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key1, 67, 38);
    }
    handleRoot();
  });

  server.on("/key2", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key2, 67, 38);
    }
    handleRoot();
  });

  server.on("/key3", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key3, 67, 38);
    }
    handleRoot();
  });

  server.on("/key4", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key4, 67, 38);
    }
    handleRoot();
  });

  server.on("/key5", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key5, 67, 38);
    }
    handleRoot();
  });

  server.on("/key6", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key6, 67, 38);
    }
    handleRoot();
  });

  server.on("/key7", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key7, 67, 38);
    }
    handleRoot();
  });

  server.on("/key8", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key8, 67, 38);
    }
    handleRoot();
  });

  server.on("/key9", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key9, 67, 38);
    }
    handleRoot();
  });

  server.on("/key0", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(key0, 67, 38);
    }
    handleRoot();
  });

  server.on("/key100", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(hundred, 67, 38);
    }
    handleRoot();
  });

  server.on("/chBack", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(channelBack, 67, 38);
    }
    handleRoot();
  });

  server.on("/keyUp", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(up, 67, 38);
    }
    handleRoot();
  });

  server.on("/keyDown", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(down, 67, 38);
    }
    handleRoot();
  });

  server.on("/keyLeft", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(left, 67, 38);
    }
    handleRoot();
  });

  server.on("/keyRight", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(right, 67, 38);
    }
    handleRoot();
  });

  server.on("/keyOK", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(ok, 67, 38);
    }
    handleRoot();
  });

  server.on("/keyChUp", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(chUp, 67, 38);
    }
    handleRoot();
  });

  server.on("/keyChDown", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(chDown, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVVolUp", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(volUp, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVVolDown", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(volDown, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVDir", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(directory, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVCHTable", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(chTable, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVMute", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(mute, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVExit", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(exit1, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVRed", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(red, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVGreen", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(green, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVYellow", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(yellow, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVBlue", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(blue, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVVideoMode", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(videoMode, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVAudioMode", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(audioMode, 67, 38);
    }
    handleRoot();
  });
  server.onNotFound(handleNotFound);
}
//**********************Loop **************
void loop() {
  server.handleClient();
  webSocket.loop();

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    i ++;
    if (i > 5) {
      ESP.deepSleep(30000000, WAKE_RF_DEFAULT);
    }
  }
  //***********Call Deep Sleep DB and Start Deep Sleep ***********
  String url = "http://";
  url += serverIP;
  url += "/deepSelect.php?entity=";
  url += entity;
  // if((WiFiMulti.run() == WL_CONNECTED)) {

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
    //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    ESP.deepSleep(30000000, WAKE_RF_DEFAULT);
  }
  http.end();
  //}
  //**********end Check DB for deep sleep *********************
  //delay(50);
}
