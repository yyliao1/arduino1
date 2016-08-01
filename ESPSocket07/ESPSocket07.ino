#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
//#include <FS.h>

ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "YYap";
const char* password = "1111111111";
const char* host = "lamp01";
IPAddress ip(192, 168, 1, 111); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

#define LEDPIN D0

#define IN false
#define OUT true
#define HI true
#define LO false
uint16_t g_gpiooutputmask = 0;

ESP8266WebServer server(80);

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

        // send message to client
        webSocket.sendTXT(num, "C");
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

        case 'p': // ping, will reply pong
          Serial.printf("[%u] Got message: %s\n", num, payload);
          webSocket.sendTXT(0, "pong");
          break;

        case 'e': case 'E':   //Echo
          webSocket.sendTXT(0, text);

        case 'G': // GPIO
          {
           Serial.printf("[%u] Got message: %s\n", num, payload);
            int pin = payload[1] - '0';
            int value;

            value = payload[2] - '0';


            pinMode(pin, OUTPUT);
              digitalWrite(pin, value);

             Serial.printf("[%u] Got message: %s\n", num, payload);
             webSocket.sendTXT(0,"SWITCH");
            // break;

            // nr = atoi( &textC[2] );
            nr = atoi( &text[2] );
            // nr = my_atoi( &textC[2] );

            if ( AFMapper[nr] == 1 ) // Not a GPIO
            {
              Serial.printf("[%u] Sorry, not a GPIO: %s\n", num, payload);
              ets_sprintf( b, "!G%c%d\n", payload[1], nr );
              webSocket.sendTXT(0, b);
              break;
            }
            else if ( AFMapper[nr] ) // It is a GPIO pin
            {
              PIN_FUNC_SELECT( AFMapper[nr], 3);  //Select AF pin to be GPIO.
            }

            switch ( payload[1] )
            {
              case '0':
              case '1':
                Serial.printf("[%u] GPIO Set/Reset request: %s nr: %d\n", num, payload, nr);
                // pinMode(GPIO_ID_PIN(nr), OUTPUT);
                digitalWrite(GPIO_ID_PIN(nr), payload[1] - '0' );
                ets_sprintf( b, "G%c%d", payload[1], nr );
                g_gpiooutputmask |= (1 << nr);
                break;

              case 'i': case 'I': // Not sure why this command is necessary when GS exists and works without clicking
                Serial.printf("[%u] GPIO Input request: %s nr: %d\n", num, payload, nr);
                pinMode(GPIO_ID_PIN(nr), INPUT);
                ets_sprintf( b, "GI%d\n", nr );
                g_gpiooutputmask &= ~(1 << nr);
                break;

              case 'f': case 'F':
                {
                  Serial.printf("[%u] GPIO fiddle request: %s nr: %d\n", num, payload, nr);
                  on = digitalRead( GPIO_ID_PIN(nr) );
                  on = !on;
                  pinMode(GPIO_ID_PIN(nr), OUTPUT); // Not sure why this is necessary?
                  digitalWrite(GPIO_ID_PIN(nr), on );
                  g_gpiooutputmask |= (1 << nr);
                  ets_sprintf( b, "GF%d\t%d\n", nr, on );
                  break;
                }

              case 'g': case 'G':
                Serial.printf("[%u] GPIO Get request:: %s\n", num, payload);
                ets_sprintf( b, "GG%d\t%d\n", nr, digitalRead( GPIO_ID_PIN(nr) ) );
                break;

              case 's': case 'S':
                {
                  // Serial.printf("[%u] GPIO Status request:: %s\n", num, payload);
                  rmask = 0;
                  for ( i = 0; i < 16; i++ )
                  {
                    rmask |= digitalRead( GPIO_ID_PIN(i) ) ? (1 << i) : 0;
                  }
                  ets_sprintf( b, "GS\t%d\t%d\n", g_gpiooutputmask, rmask );
                  break;
                }

              default:
                Serial.printf("[%u] GPIO Unknown request:: %s\n", num, payload);
                break;
            }
            webSocket.sendTXT(0, b);
          }
          break;

        case 'S': // Slider
          {
            webSocket.sendTXT(0, text); // For now just echo back
          }

        default:
          webSocket.sendTXT(0, "**** UNDEFINED ****");
          Serial.printf("[%u] Got UNDEFINED message: %s\n", num, payload);
          break;

      }
      break;

    case WStype_BIN:
      Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
      hexdump(payload, lenght);

      // send message to client
      // webSocket.sendBIN(num, payload, lenght);
      break;
  }

}





void setup() {
  Serial.begin(115200);
  Serial.print("\n");

 digitalWrite(LED_BUILTIN, HIGH);
  //pinMode(LEDPIN, OUTPUT);


  // Connect tp Wifi
  Serial.printf("Connecting to %s\n", ssid);
 // WiFi.mode(WIFI_STA);
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
  Serial.print("Open http://");
  Serial.print(host);
  Serial.println(".local/edit to see the file browser");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // WiFi.begin(ssid, password);
  // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //   Serial.println("Connection Failed! Rebooting...");
  //   delay(5000);
  //   ESP.restart();
  // }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


 
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {

  server.handleClient();

  webSocket.loop();

}
