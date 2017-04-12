#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "./config.h"
#include "./helpers.h"

WiFiClient client;
WiFiServer server(23);
struct State {
  int motor_l;
  int motor_r;
  int servo;
} state;

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

    // Serial.println("Waiting for cmd...");

    String cmd = client.readStringUntil('\r');

    //waiting for cmd "funcName:100:200:300:"

    if (cmd.length()) {
        client.flush();
        // Serial.print(cmd.length());
        // Serial.print(" Received cmd: ");
        // Serial.println(cmd);

        state = ParseCmd(cmd);

        Serial.print("STATE: motor_l: ");
        Serial.print(state.motor_l);
        Serial.print(" motor_r: ");
        Serial.print(state.motor_r);
        Serial.print(" servo: ");
        Serial.println(state.servo);

    }

    // switch (1) {
    //     case (cmd == "on"): {
    //         digitalWrite(LED_PIN, LOW);
    //         client.println("== ok ==");
    //     } break;
    //     case (cmd == "off"): {
    //         digitalWrite(LED_PIN, HIGH);
    //         client.println("== ok ==");
    //     } break;
    //     default: {
    //         client.println("== error ==");
    //     } break;
    // }

    // Serial.println("do again!");
}

State ParseCmd(String cmd) {
    State data;
    String func = "";
    int args[10];
    int argsCount = 0;




    String separator = ":";
    int partFrom = cmd.indexOf(separator);
    int partTo = 0;

    if (partFrom >= 0) {
        func = cmd.substring(0, partFrom++);
    }

    while ((partTo = cmd.indexOf(separator, partFrom)) && partTo >= 0) {
        String val = cmd.substring(partFrom, partTo);
        args[argsCount] = val.toInt();
        argsCount++;
        partFrom = partTo + 1;
    }

    Serial.print("ParseCmd. Func: ");
    Serial.println(func);
    Serial.print("argsCount: ");
    Serial.println(argsCount);
    for (int i = 0; i < argsCount; i++) {
        Serial.print(args[i]);
        Serial.print(" ");
    }
    Serial.println("parseEnd");

    data.motor_l = args[0];
    data.motor_r = args[1];
    data.servo = args[2];

    return data;
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
    if (!client.print(".")) {
        Serial.println(" [client lost]");
        client.stop();
        return;
    }
}
