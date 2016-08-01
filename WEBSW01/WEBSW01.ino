
#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
//#include <ESP8266HTTPUpdateServer.h>

const char* ssid     = "YYap";
const char* password = "1111111111";
//IPAddress ip(192, 168, 1, 111); //Node static IP
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 255, 0);

const char* host1 = "192.168.1.102"; //PHP APACHE

const char* host = "SW01"; //ESP8266

//ESP8266WebServer httpServer(80);
//ESP8266HTTPUpdateServer httpUpdater;

void setup() {

  Serial.begin(115200);
 // delay(1000);
  Serial.println("");
  Serial.println("");
  Serial.println("Begin");
//  delay(1000);
  pinMode(5, INPUT);

 // WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//    WiFi.begin(ssid, password);
//   Serial.println("WiFi failed, retrying.");
 // }

  //MDNS.begin(host);

 // httpUpdater.setup(&httpServer);
 // httpServer.begin();

 // MDNS.addService("http", "tcp", 80);
 // Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
}

void loop() {

 // httpServer.handleClient();
  
  Serial.println("Loop");
  Serial.println("data");
  Serial.println(digitalRead(5));

  Serial.print("connecting to ");
  Serial.println(host1);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  // client.connect('http://localhost/test3.php?event=EVENT&entity=ENTITY&measure=100.4',80);
  if (!client.connect(host1, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // We now create a URI for the request
  //http://localhost/test3.php?event=EVENT&entity=ENTITY&measure=100.4
  String url = "http://192.168.1.102/test3.php?event=CHANGE&entity=SW01&measure=";

  // url += "&value=";
  url += digitalRead(5);

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host1 + "\r\n" +
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
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");


  ESP.deepSleep(20000000, WAKE_RF_DEFAULT);
 
 //   delay(10000);
}
