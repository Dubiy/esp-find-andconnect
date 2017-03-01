#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "./config.h"
#include "./helpers.h"

WiFiClient client;
WiFiServer server(23);
char s[100];

void setup() {
  //TODO rewrite loading params from configuration
  char ssid[] = "happyFamily";
  char pass[] = "12345676";

  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Serial.println();

setupAp();
  //
  // Serial.print("Trying to connect to ");
  // Serial.println(ssid);
  //
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, pass);
  //
  // int status = WiFi.waitForConnectResult();
  // if (status != WL_CONNECTED) {
  //   //setup module as AP
  //   Serial.println("Connection Failed. Starting AP...");
  //   setupAp();
  // } else {
  //   //connected to Controller
  //   Serial.println("Connected.");
  //
  //   showConnectionInfo();
  // }

  // IPAddress ip(192, 168, 1, 129);
  // if (!client.connect(ip, 23)) {
  //   Serial.println("Connection to echo server failed");
  //   // while (true) {}
  // }

  server.begin();
  server.setNoDelay(true);
}

void loop() {
  while (!client) {
    client = server.available();
    Serial.print(".");
    delay(500);
  }
  if (!client.connected()) {
    Serial.println("Client disconnected");
    client.stop();
    return;
  }

  Serial.println("Waiting for cmd...");

  getstr(s, sizeof(s));
  Serial.print("Received cmd: ");
  Serial.println(s);

  // process cmd and send result to echo server

  if (strcmp("on", s) == 0) {
    digitalWrite(LED_PIN, LOW);
    client.println("== ok ==");
  } else if (strcmp("off", s) == 0) {
    digitalWrite(LED_PIN, HIGH);
    client.println("== ok ==");
  } else {
    client.println("== error ==");
  }
  Serial.println("do again!");
}

char* getstr(char* s, int maxlen) {
  int i = 0;
  strcpy(s, "");

  while (true) {
    while (client.available() == 0) {
        //https://github.com/esp8266/Arduino/blob/master/doc/reference.md#timing-and-delays
        // No infinite null loops allowed! Delay needs to be used which
        // yields control to ESP routines as necessary.
        client.flush();
        delay(300);
        Serial.print('^');

        // or
        // yield();
        if (!client.connected()) {
          return "";
        }
    }
    Serial.print('-');
    s[i] = client.read();
    Serial.print('+');
    if (s[i] == '\n') {
        s[i] = '\0';
        break; // while
    } else if (s[i] == '\r') {
       // do nothing additional on return character - it will be overwritten by next char
    } else {
      if (i >= maxlen) {
        Serial.print("buffer ");
        Serial.print(maxlen);
        Serial.print(" overflow: ");
        Serial.println(s);
        // while (true) {}
      } else {
        i++;
      }
    }
  }

  return s;
}
