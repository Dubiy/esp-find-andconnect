#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "./config.h"
#include "./helpers.h"

WiFiClient client;
WiFiServer server(23);
char cmd[100];

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
    GetClient();

    Serial.println("Waiting for cmd...");

    // String Message = client.readStringUntil('\r');

    getstr(cmd, sizeof(cmd));
    Serial.print("Received cmd: ");
    Serial.println(Message);

    // process cmd and send result to echo server

    if (strcmp("on", cmd) == 0) {
        digitalWrite(LED_PIN, LOW);
        client.println("== ok ==");
    } else if (strcmp("off", cmd) == 0) {
        digitalWrite(LED_PIN, HIGH);
        client.println("== ok ==");
    } else {
        client.println("== error ==");
    }
    Serial.println("do again!");
}

char* getstr(char* str, int maxlen) {
    unsigned long previousMillis = 0, currentMillis = 0;
    long INTERVAL_ping = 5000;
    int i = 0;

    strcpy(str, "");

    while (true) {
        while (client.available() == 0) {
            client.flush();
            //https://github.com/esp8266/Arduino/blob/master/doc/reference.md#timing-and-delays
            // No infinite null loops allowed! Delay needs to be used which
            // yields control to ESP routines as necessary.

            delay(30); // or yield();

            currentMillis = millis();
            if (currentMillis - previousMillis > INTERVAL_ping || previousMillis > currentMillis) {
                //previousMillis > currentMillis: It just show the elapsed time from the start of the board. This number will overflow (go back to zero), after approximately 50 days (source: arduino.cc/en/Reference/millis)
                previousMillis = currentMillis;

                Serial.print('.');
                if (!client.print(".")) {
                    Serial.println(" [client lost]");
                    client.stop();
                    return "";
                }
            }

            if (!client.connected()) {
                return "";
            }
        }

        str[i] = client.read();
        Serial.print(str[i]);
        if (str[i] == '\n' || str[i] == '\r') {
            str[i] = '\0';
            break; // while
        } else {
            if (i >= maxlen) {
                Serial.print("buffer ");
                Serial.print(maxlen);
                Serial.print(" overflow: ");
                Serial.println(str);
            } else {
                i++;
            }
        }
    }

    return str;
}

void GetClient() {
    while (!client || !client.connected()) {
        if (client) {
            client.stop();
        }
        client = server.available();
        Serial.print(".");
        delay(500);
    }
    if (!client.connected()) {
        Serial.println("Client disconnected");
        client.stop();
        return;
    }
    // if (!client.print(".")) {
    //     Serial.println(" [client lost]");
    //     client.stop();
    //     return;
    // }
}
