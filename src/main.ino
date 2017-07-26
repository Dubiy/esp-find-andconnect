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

const int MOTOR_L1 = 5; // D1
const int MOTOR_L2 = 4; // D2
const int MOTOR_R1 = 0; // D3
const int MOTOR_R2 = 2; // D4
const int BUZZER = 14; // D5
const int SERVO =  15;  // D8
const bool DEBUG = false;

void setup() {
    //TODO rewrite loading params from configuration
    char ssid[] = "happyFamily";
    char pass[] = "12345676";

    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    pinMode(MOTOR_L1, OUTPUT);
    pinMode(MOTOR_L2, OUTPUT);
    pinMode(MOTOR_R1, OUTPUT);
    pinMode(MOTOR_R2, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    // servo.attach(SERVO);

    Serial.println();

    Serial.print("Trying to connect to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    int status = WiFi.waitForConnectResult();
    if (status != WL_CONNECTED) {
      //setup module as AP
      Serial.println("Connection Failed. Starting AP...");
      setupAp();
    } else {
      //connected to Controller
      Serial.println("Connected.");

      showConnectionInfo();
    }

    // IPAddress ip(192, 168, 1, 129);
    // if (!client.connect(ip, 23)) {
    //   Serial.println("Connection to echo server failed");
      // while (true) {}
    // }

    server.begin();
    server.setNoDelay(true);
}


int motor_l = 0;
int motor_r = 0;
int servo_pos = 0;

void loop() {
    GetClient();

    // Serial.println("Waiting for cmd...");

    String cmd = client.readStringUntil('\r');
    //waiting for cmd "funcName:100:200:300:"

    Serial.print("cmd: ");
    Serial.println(cmd);

    if (cmd.length()) {
        client.flush();

        state = ParseCmd(cmd);

        setMotorState(state.motor_l, state.motor_r);
        // servo.write(state.servo);

        // Serial.print("STATE: motor_l: ");
        // Serial.print(state.motor_l);
        // Serial.print(" motor_r: ");
        // Serial.print(state.motor_r);
        // Serial.print(" servo: ");
        // Serial.println(state.servo);

    }
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

    // Serial.print("ParseCmd. Func: ");
    // Serial.println(func);
    // Serial.print("argsCount: ");
    // Serial.println(argsCount);
    // for (int i = 0; i < argsCount; i++) {
        // Serial.print(args[i]);
        // Serial.print(" ");
    // }
    // Serial.println("parseEnd");

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
    Serial.println("connected!");
}

void setMotorState(int motor_l, int motor_r) {
  int motor_l1 = 0,
      motor_l2 = 0,
      motor_r1 = 0,
      motor_r2 = 0;

  if (motor_l > 0) {
    motor_l1 = motor_l;
  } else {
    motor_l2 = abs(motor_l);
  }

  if (motor_r > 0) {
    motor_r1 = motor_r;
  } else {
    motor_r2 = abs(motor_r);
  }

  if (DEBUG) {
    Serial.print("motor_l1 = ");
    Serial.print(motor_l1);
    Serial.print(". motor_l2 = ");
    Serial.print(motor_l2);
    Serial.print(". motor_r1 = ");
    Serial.print(motor_r1);
    Serial.print(". motor_r2 = ");
    Serial.println(motor_r2);
  }

  analogWrite(MOTOR_L1, motor_l1);
  analogWrite(MOTOR_L2, motor_l2);
  analogWrite(MOTOR_R1, motor_r1);
  analogWrite(MOTOR_R2, motor_r2);
}
