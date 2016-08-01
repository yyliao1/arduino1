/* @file MultiKey.ino
  || @version 1.0
  || @author Mark Stanley
  || @contact mstanley@technologist.com
  ||
  || @description
  || | The latest version, 3.0, of the keypad library supports up to 10
  || | active keys all being pressed at the same time. This sketch is an
  || | example of how you can get multiple key presses from a keypad or
  || | keyboard.
  || #
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <Keypad.h>
#include <string.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;


const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = { 5, 4, 2, 14 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = {12, 13, 3}; //connect to the column pinouts of the keypad
//byte rowPins[ROWS] = { 3, 13, 12, 14 }; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {5, 4, 2}; //connect to the column pinouts of the keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount;
unsigned long startTime;
String msg;

void openSocket(char * yy) {
  webSocket.sendTXT(yy);
  Serial.println(yy);
  //Serial.printf("[WSc] Connected to url: %s\n",  payload);
}
void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {

  Serial.println("AAA");
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n",  payload);

        // send message to server when Connected
        // webSocket.sendTXT("G51");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary lenght: %u\n", lenght);
      hexdump(payload, lenght);

      // send data to server
      // webSocket.sendBIN(payload, lenght);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  loopCount = 0;
  startTime = millis();
  msg = "";

  WiFiMulti.addAP("YYap", "1111111111");

  //WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  webSocket.begin("192.168.1.112", 81);
  //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  loopCount++;
  if ( (millis() - startTime) > 5000 ) {
    Serial.printf("Average loops per second = ");
    Serial.println(loopCount / 5);
    startTime = millis();
    loopCount = 0;
  }

  // Fills kpd.key[ ] array with up-to 10 active keys.
  // Returns true if there are ANY active keys.
  if (kpd.getKeys())
  {

    for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED:
            msg = " PRESSED.";
            break;
          case HOLD:
            msg = " HOLD.";
            break;
          case RELEASED:
            msg = " RELEASED.";
            break;
          case IDLE:
            msg = " IDLE.";
        }
        Serial.printf("Key \n");
        Serial.print(kpd.key[i].kchar);
        Serial.println(msg);
        //char aa=kpd.key[i].kchar;
        //Serial.println(aa);
        // webSocket.onEvent(webSocketEvent);
        
       if (kpd.key[i].kchar == '1')
       {
          openSocket("G51");
       }
  if (kpd.key[i].kchar == '2')
       {
          openSocket("G50");
       }
      }
    }
  }

}  // End loop
