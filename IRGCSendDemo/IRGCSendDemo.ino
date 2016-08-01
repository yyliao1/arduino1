/*
   IRremoteESP8266: IRsendGCDemo - demonstrates sending Global Cache-formatted IR codes with IRsend
   An IR LED must be connected to ESP8266 pin 0.
   Version 0.1 30 March, 2016
   Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009, Copyright 2009 Ken Shirriff, http://arcfn.com
*/

#include <IRremoteESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// ******* httpupdater
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>


// Codes are in Global Cache format less the emitter ID and request ID. These codes can be found in GC's Control Tower database.
unsigned int  rawDown[67] = {9200,4300, 750,400, 750,400, 750,400, 750,400, 750,400, 750,450, 750,450, 750,450, 750,1550, 700,1600, 750,1550, 700,1550, 750,1550, 750,1550, 750,1550, 750,1550, 750,450, 750,450, 750,1550, 700,1550, 750,1550, 700,450, 750,450, 700,450, 700,1550, 700,1600, 700,450, 750,450, 700,450, 700,1550, 700,1550, 700,1600, 700};  // NEC FF38C7
unsigned int  rawUp[67] = {9150,4300, 750,450, 800,350, 850,350, 750,400, 800,350, 800,400, 750,450, 800,350, 750,1550, 750,1500, 750,1550, 750,1500, 750,1550, 750,1550, 750,1500, 750,1550, 750,450, 750,400, 750,450, 750,1550, 750,1550, 750,450, 750,450, 700,450, 700,1600, 700,1550, 700,1550, 750,450, 750,450, 750,1550, 700,1550, 700,1550, 700};  // NEC FF18E7
unsigned int  rawMute[67] = {9150,4350, 750,400, 750,400, 800,400, 800,400, 750,450, 750,400, 750,450, 750,400, 750,1550, 750,1550, 750,1500, 750,1550, 750,1500, 750,1500, 750,1550, 700,1550, 700,450, 750,450, 750,450, 700,450, 750,1550, 700,450, 750,450, 750,450, 700,1600, 700,1600, 700,1600, 700,1550, 750,450, 700,1550, 700,1600, 700,1550, 700};  // NEC FF08F7
unsigned int  rawLeft[67] = {9200,4300, 800,400, 750,400, 800,400, 800,400, 750,450, 750,400, 750,400, 750,450, 750,1550, 750,1500, 750,1550, 750,1550, 750,1550, 700,1600, 750,1500, 700,1550, 750,1550, 750,450, 750,450, 700,1550, 750,1550, 750,450, 700,450, 700,450, 700,450, 700,1550, 750,1550, 700,450, 700,450, 700,1600, 700,1550, 700,1550, 750};  // NEC FF9867
unsigned int  rawOK[67] = {9200,4300, 800,400, 800,400, 800,400, 750,450, 800,400, 750,450, 750,450, 750,450, 750,1550, 800,1500, 750,1550, 750,1500, 750,1500, 750,1550, 750,1550, 750,1550, 750,400, 750,1550, 750,450, 750,1550, 750,1550, 750,450, 700,450, 750,450, 750,1550, 750,450, 700,1550, 700,450, 700,450, 750,1550, 700,1600, 700,1550, 750};  // NEC FF58A7
unsigned int  rawRight[67] = {9200,4250, 750,450, 700,450, 750,400, 750,450, 750,450, 700,450, 700,500, 650,500, 650,1600, 600,1650, 700,1600, 650,1600, 700,1550, 650,1600, 650,1600, 650,1600, 650,1600, 650,1600, 650,500, 650,1600, 650,1600, 650,500, 650,550, 650,550, 650,500, 650,550, 650,1600, 600,550, 650,500, 650,1600, 650,1600, 600,1650, 650};  // NEC FFD827
unsigned int  rawPower[67] = {9200,4250, 800,350, 800,400, 750,400, 750,400, 800,400, 750,400, 750,400, 800,400, 750,1550, 750,1550, 750,1500, 700,1600, 700,1550, 750,1500, 700,1600, 750,1550, 750,450, 700,1550, 750,450, 750,1550, 750,450, 700,450, 750,450, 700,450, 700,1550, 700,450, 700,1600, 700,450, 700,1550, 750,1550, 700,1600, 700,1550, 700};  // NEC FF50AF
unsigned int  rawPower1[67] = {9200,4250, 750,400, 750,400, 750,450, 750,400, 800,350, 700,450, 800,400, 750,400, 750,1550, 750,1550, 750,1550, 750,1550, 750,1550, 750,1550, 750,1550, 750,1550, 750,450, 750,1500, 750,450, 700,1600, 750,450, 700,450, 750,450, 750,450, 750,1550, 700,450, 700,1550, 750,450, 700,1550, 700,1550, 700,1600, 700,1550, 700};  // NEC FF50AF

unsigned int  sanyoPower[67] = {9200,4300, 750,400, 700,500, 750,400, 750,1550, 750,1550, 700,1600, 750,400, 700,500, 750,1550, 700,1600, 750,1550, 700,450, 750,400, 650,550, 750,1550, 700,1600, 750,450, 650,1650, 750,400, 700,500, 700,1600, 650,550, 700,450, 750,450, 650,1650, 700,450, 600,1650, 750,1600, 700,450, 750,1600, 700,1600, 600,1650, 700};  // NEC 1CE348B7
unsigned int  sanyoPower1[67] = {9150,4400, 650,600, 600,550, 650,500, 650,1600, 700,1600, 650,1600, 700,600, 600,500, 650,1650, 650,1600, 700,1600, 700,500, 600,600, 550,600, 550,1650, 650,1600, 700,550, 650,1550, 700,550, 600,550, 550,1650, 650,600, 600,650, 550,600, 550,1700, 650,550, 650,1650, 600,1650, 650,600, 600,1600, 600,1700, 600,1750, 550};  // JVC 1CE3
unsigned int  sanyoPower2[67] = {9200,4350, 750,450, 750,400, 750,400, 750,1550, 750,1550, 750,1550, 700,500, 700,450, 800,1500, 750,1550, 700,1600, 750,450, 700,500, 700,450, 750,1600, 700,1600, 700,500, 700,1600, 700,500, 700,450, 700,1600, 700,450, 700,450, 700,500, 700,1600, 700,500, 700,1600, 650,1650, 700,450, 700,1600, 700,1600, 700,1600, 700};  // NEC 1CE348B7

unsigned int  IBT12831[67] = {8950,4500, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550};  // UNKNOWN 8AB3679B
unsigned int  IBT12832[67] = {8950,4500, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550};  // UNKNOWN 12209C7B
unsigned int  IBT12833[67] = {8900,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,600, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550};  // UNKNOWN 305D6FF
unsigned int  IBT12834[67] = {8950,4500, 550,650, 550,650, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550};  // UNKNOWN 51E43D1B
unsigned int  IBT12835[67] = {8950,4500, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN AB91951F
unsigned int  IBT12836[67] = {8950,4500, 550,650, 550,600, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,1750, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550};  // UNKNOWN FF9186B7
unsigned int  IBT12837[67] = {8900,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550};  // UNKNOWN F076C13B
unsigned int  IBT12838[67] = {8900,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN 8C22657B
unsigned int  IBT12839[67] = {8950,4500, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,600, 550,650, 550,1700, 550,1700, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550};  // UNKNOWN E5CFBD7F
unsigned int  IBT128310[67] = {8950,4500, 550,600, 550,650, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,650, 550,1750, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550};  // UNKNOWN 9716BE3F
unsigned int  IBT12830[67] = {8950,4500, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,600, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN FA3F159F
unsigned int  IBT128311[67] = {8950,4500, 550,650, 600,600, 600,600, 600,600, 600,600, 600,600, 600,600, 600,600, 600,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,1700, 550,1700, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN 44C407DB

unsigned int  IBT1283Mute[67] = {8950,4550, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,600, 550,650, 550,650, 550,1700, 550,650, 550,600, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN 45473C1B
unsigned int  IBT1283Stop[67] = {8950,4500, 550,650, 550,600, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,650, 550,1750, 550,650, 550,600, 550,650, 550,1700, 550,600, 550,1750, 550,1700, 550,650, 550,1700, 550,1750, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN 52A3D41F
unsigned int  IBT1283Play[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,600, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,1700, 550,1700, 550,650, 550,1750, 550};  // UNKNOWN E318261B
unsigned int  IBT1283PreSect[67] = {8950,4500, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN D7E84B1B
unsigned int  IBT1283Backward[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN 3195A31F
unsigned int  IBT1283Forward[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,1750, 550};  // UNKNOWN 32C6FDF7
unsigned int  IBT1283Info[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,600, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,600, 550,1700, 550};  // UNKNOWN 8A33665B
unsigned int  IBT1283Pause[67] = {8900,4550, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN EE886D7F
unsigned int  IBT1283BackTo[67] = {8950,4500, 550,650, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,600, 550,1750, 550,1700, 550,650, 550,1700, 550,650, 550,600, 550,600, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,1700, 550,1700, 550};  // UNKNOWN C101E57B
unsigned int  IBT1283Menu[67] = {8900,4550, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550};  // UNKNOWN A6C4637B
unsigned int  IBT1283MultiAudio[67] = {8950,4500, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,650, 550,600, 550,1700, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550,1700, 550,1750, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN A8E05FBB
unsigned int  IBT1283Word[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,650, 550,1700, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,1750, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550};  // UNKNOWN 3954B1B7
unsigned int  IBT1283VolUp[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,1750, 550,650, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN F377C5B7
unsigned int  IBT1283VolDown[67] = {8950,4500, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550,1700, 550,1750, 550};  // UNKNOWN A3C8EDDB
unsigned int  IBT1283Home[67] = {8950,4500, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,650, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN 8E5D3EBB
unsigned int  IBT1283DTVAV[67] = {8950,4550, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,1750, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550};  // UNKNOWN D538681B
unsigned int  IBT1283FactorySetup[67] = {8900,4550, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,650, 550,1700, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550};  // UNKNOWN 9716BE3F
unsigned int  IBT1283Browser[67] = {8950,4500, 550,650, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550,1700, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550};  // UNKNOWN 44C407DB
unsigned int  IBT1283Program[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550,1700, 550,600, 550,1700, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN 930FFB7
unsigned int  IBT1283Channel[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550};  // UNKNOWN 1BC0157B
unsigned int  IBT1283RecordList[67] = {8950,4500, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,1700, 550,1700, 550,650, 550,650, 550,600, 550,650, 550,650, 550,1700, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN F0C41643
unsigned int  IBT1283TTX[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,1750, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,1750, 550};  // UNKNOWN B5310E1F
unsigned int  IBT1283Zoomout[67] = {8900,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,600, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,1700, 550,1700, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,1700, 550,650, 550,650, 550,1700, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN 7EC31EF7
unsigned int  IBT1283Zoomin[67] = {8950,4500, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,600, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,600, 550,1700, 550,1700, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN 9BFC1697
unsigned int  IBT1283Repeat[67] = {8950,4500, 550,600, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,650, 550,1700, 550,600, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN EC213597
unsigned int  IBT1283RepratAB[67] = {8900,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,650, 550,1750, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,1700, 550,650, 550,650, 550,1700, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN EE4ECCFB
unsigned int  IBT1283Resolution[67] = {8900,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550};  // UNKNOWN B953793F
unsigned int  IBT1283LRSpeaker[67] = {8950,4500, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,1750, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,600, 550,1700, 550,650, 550,1750, 550};  // UNKNOWN B1A7211B
unsigned int  IBT1283Delete[67] = {8950,4500, 550,600, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,600, 550,1700, 550,1700, 550,1700, 550,650, 550,1700, 550,1700, 550,1750, 550};  // UNKNOWN 9EF4941F
unsigned int  IBT1283Edit[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550,650, 550,1700, 550,1750, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN 5B83B61B

unsigned int  IBT1283Power[67] = {8950,4500, 550,650, 550,600, 550,600, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550,650, 550,600, 550,600, 550,650, 550,1750, 550,650, 550,1700, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550};  // UNKNOWN 2A89195F
unsigned int  IBT1283PlayPause[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550,1700, 550,650, 550,650, 550,600, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,650, 550,1750, 550};  // UNKNOWN 511DBB
unsigned int  IBT1283NextSect[67] = {8900,4550, 550,600, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550};  // UNKNOWN 20FE4DBB
unsigned int  IBT1283Record[67] = {8950,4550, 550,600, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,1700, 550,650, 550,1700, 550,650, 550,1750, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,1700, 550,1700, 550,650, 550,1700, 550};  // UNKNOWN 3EC3FC1B
unsigned int  IBT1283Right[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,650, 550,1700, 550,1700, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,650, 550,650, 550,1700, 550,1700, 550,1700, 550};  // UNKNOWN 86B0E697
unsigned int  IBT1283ViewAngel[67] = {8950,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,1700, 550,650, 550,1700, 550,1750, 550,600, 550,1700, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,1700, 550,650, 550,1750, 550};  // UNKNOWN 449E79F
unsigned int  IBT1283ChanelUp[67] = {8950,4500, 550,650, 550,650, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,650, 550,650, 550,600, 550,1700, 550,1700, 550,650, 550,1750, 550,650, 550,1700, 550,1700, 550,1750, 550};  // UNKNOWN 13549BDF
unsigned int  IBT1283ChanelDown[67] = {8900,4550, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,650, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,1700, 550,1750, 550,1700, 550,600, 550,1700, 550,650, 550,600, 550,650, 550,650, 550,650, 550,650, 550,1700, 550,650, 550,1700, 550,1750, 550,1700, 550};  // UNKNOWN 5BE75E7F



IRsend irsend(5); //an IR emitter led is connected to GPIO pin 4
//http updater
const char* host = "irremote";

const char* ssid = "YYap";
const char* password = "1111111111";

IPAddress ip(192, 168, 1, 110); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Global variables
WiFiClient client;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleRoot() {
  // Prepare the response. Start with the common header:
  String s = "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>";
  s += " <meta name=\"viewport\" content=\"user-scalable=no, initial-scale=1, maximum-scale=1, minimum-scale=1, width=device-width\">";
  s += "<img src=\"http://yyliao1.github.io/image/ibt1283-1.jpg\" width=\"400\" height=\"490\" usemap=\"#TestMap\">";
  s += "<map name=\"TestMap\">";
  s += "<area shape=\"rect\" coords=\" 13 , 13 , 58 , 53\" href=\"\\TVMute\">";
  s += "<area shape=\"rect\" coords=\" 138 , 18 , 181 , 56\" href=\"\\TVOn\">";
  s += "<area shape=\"rect\" coords=\" 194 , 1 , 244 , 43\" href=\"\\key1\">";
  s += "<area shape\"rect\" coords=\" 271 , 1 , 330 , 43\" href=\"\\key2\">";
  s += "<area shape\"rect\" coords=\" 354 , 1 , 397 , 49\" href=\"\\key3\">";
  s += "<area shape\"rect\" coords=\" 190 , 53 , 243 , 94\" href=\"\\key4\">";
  s += "<area shape\"rect\" coords=\" 267 , 53 , 324 , 97\" href=\"\\key5\">";
  s += "<area shape\"rect\" coords=\" 348 , 58 , 396 , 103\" href=\"\\key6\">";
  s += "<area shape\"rect\" coords=\" 192 , 107 , 239 , 146\" href=\"\\key7\">";
  s += "<area shape\"rect\" coords=\" 269 , 107 , 317 , 150\" href=\"\\key8\">";
  s += "<area shape\"rect\" coords=\" 349 , 107 , 398 , 151\" href=\"\\key9\">";
  s += "<area shape\"rect\" coords=\" 268 , 158 , 315 , 203\" href=\"\\key0\">";

  s += "<area shape\"rect\" coords=\" 58 , 220 , 122 , 249\" href=\"\\keyUp\">";
  s += "<area shape\"rect\" coords=\" 58 , 309 , 117 , 341\" href=\"\\keyDown\">";
  s += "<area shape\"rect\" coords=\" 26 , 248 , 56 , 313\" href=\"\\keyLeft\">";
  s += "<area shape\"rect\" coords=\" 123 , 251 , 147 , 317\" href=\"\\keyRight\">";
  s += "<area shape\"rect\" coords=\" 67 , 258 , 109 , 305\" href=\"\\keyOK\">";

  s += "<area shape\"rect\" coords=\" 135 , 388 , 158 , 434\" href=\"\\keyChUp\">";
  s += "<area shape\"rect\" coords=\" 131 , 432 , 164 , 478\" href=\"\\keyChDown\">";
  
  s += "</map>";
  s += "<style>td{font-size:xx-large;}</style>";
  s += "<table  style=\"width:100%;\">";
  s += "<tr align=\"center\"><td><a href=\"\\TVPower\"><button>TV On/Off</button></a></td>";
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


void setup()
{
  irsend.begin();
  Serial.begin(9600);

 
 while(WiFi.waitForConnectResult() != WL_CONNECTED){
    WiFi.begin(ssid, password);
     WiFi.config(ip, gateway, subnet);
    Serial.println("WiFi failed, retrying.");
  }

  MDNS.begin(host);

 //http updater
  MDNS.begin(host);
  httpUpdater.setup(&server);
  //server.begin();
 

 
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

  //  if (MDNS.begin("esp8266")) {
  //    Serial.println("MDNS responder started");
  // }


  server.on("/", handleRoot);

  server.on("/TVPower", []() {
    //  server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      //  irsend.sendRaw(sanyoPower1, 67, 38);
      irsend.sendRaw(sanyoPower2, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVMute", []() {
    //  server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(rawMute, 67, 38);
    }
    handleRoot();
  });

  server.on("/TVOn", []() {
    //   server.send(200, "text/plain", "this works as well");
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT1283Power, 67, 38);
    }
    handleRoot();
  });

  server.on("/key1", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12831, 67, 38);
    }
    handleRoot();
  });

  server.on("/key2", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12832, 67, 38);
    }
    handleRoot();
  });

  server.on("/key3", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12833, 67, 38);
    }
    handleRoot();
  });

  server.on("/key4", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12834, 67, 38);
    }
    handleRoot();
  });

  server.on("/key5", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12835, 67, 38);
    }
    handleRoot();
  });

  server.on("/key6", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12836, 67, 38);
    }
    handleRoot();
  });

  server.on("/key7", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12837, 67, 38);
    }
    handleRoot();
  });

  server.on("/key8", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12838, 67, 38);
    }
    handleRoot();
  });

  server.on("/key9", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12839, 67, 38);
    }
    handleRoot();
  });

  server.on("/key0", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT12830, 67, 38);
    }
    handleRoot();
  });

  server.on("/key10", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT128310, 67, 38);
    }
    handleRoot();
  });

  server.on("/key11", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT128311, 67, 38);
    }
    handleRoot();
  });

   server.on("/keyUp", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(rawUp, 67, 38);
    }
    handleRoot();
  });

     server.on("/keyDown", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(rawDown, 67, 38);
    }
    handleRoot();
  });

      server.on("/keyLeft", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(rawLeft, 67, 38);
    }
    handleRoot();
  });

       server.on("/keyRight", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(rawRight, 67, 38);
    }
    handleRoot();
  });

       server.on("/keyOK", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(rawOK, 67, 38);
    }
    handleRoot();
  });

         server.on("/keyChUp", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT1283ChanelUp, 67, 38);
    }
    handleRoot();
  });

          server.on("/keyChDown", []() {
    for (int i = 0; i < 3; i++) {
      irsend.sendRaw(IBT1283ChanelDown, 67, 38);
    }
    handleRoot();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.println("HTTP server started");

 

}

void loop() {
  server.handleClient();
  // Serial.println("Toggling power");

// httpServer.handleClient();

   delay(50);
}
