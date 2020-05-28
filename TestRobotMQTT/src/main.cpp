#include <M5Stack.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// for wifi
WiFiClient wifiClient;

// #define SSID "SSID"
// #define PASSWORD "password"

// command
int command = 0;
int counter = 0;

// for servo
#define RIGHT_SERVO_CH 0
#define LEFT_SERVO_CH 1

#define RIGHT_SERVO_PIN 21
#define LEFT_SERVO_PIN 22

/*=============================== */
/* MQTT */
/*=============================== */
const char *THING_NAME = "M5Stack";
PubSubClient mqttClient;

void callback(char* topic, byte* payload, unsigned int length) {
  String buf_t = String(topic);
  M5.Lcd.println(buf_t);

  payload[length] = '\0';
  String buf_s = String((char*) payload);
  M5.Lcd.println(buf_s);
    
  command = buf_s.toInt();
  M5.Lcd.printf("cmd:%d\n", command);

  counter = 0;
}

void connectToBroker() {
  M5.Lcd.println("trying to connect MQTT broker");
  mqttClient.setServer("192.168.11.9", 1883);
  mqttClient.setCallback(callback);
  mqttClient.setClient(wifiClient);
  if (!mqttClient.connect(THING_NAME)) {
    M5.Lcd.println(mqttClient.state());
  }
  mqttClient.subscribe("robot/command/#");
}

/*=============================== */
/* setup */
/*=============================== */
void setup(void) {

  M5.begin();
  Serial.begin(115200);

  // wifi connetction

  WiFi.begin(SSID, PASSWORD);
  M5.Lcd.println("Wait for WiFi... ");

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.println(".");
  }

  M5.Lcd.println("WiFi connected");
  M5.Lcd.println("IP address: ");
  M5.Lcd.println(WiFi.localIP());

  connectToBroker();
  M5.Lcd.println("success to connect MQTT broker");

  // PWMの設定
  ledcSetup(RIGHT_SERVO_CH, 50, 10); // チャネル, 周波数=50Hz, 分解能=0-1023
  ledcAttachPin(RIGHT_SERVO_PIN, RIGHT_SERVO_CH); // ピン番号, チャネル

  ledcSetup(LEFT_SERVO_CH, 50, 10); // チャネル, 周波数=50Hz, 分解能=0-1023
  ledcAttachPin(LEFT_SERVO_PIN, LEFT_SERVO_CH); // ピン番号, チャネル
}

/*=============================== */
/* servo control */
/*=============================== */
void moveServo(int channel, int angle) {
  ledcWrite(channel, map(angle, -90, 90, 26, 123));
}

// spd = (-100, 100)
void moveWheel(int channel, float spd) {
  // float _spd = spd + 1.5;
  // if (_spd < 0) {
  //   _spd = 0.0;
  // } else if (_spd > 2) {
  //   _spd = 2.0;
  // }
  // int pulseCount = int(_spd * 1023 / 20);

  // min = 0 * 1023 / 20
  // max = 2.0 * 1023 / 20

  ledcWrite(channel, map(spd, -100, 100, 1, 102));
}

/*=============================== */
/* main loop */
/*=============================== */
void loop() {
  mqttClient.loop();

  float spd = 15;
  float duration_count = 15000;
  float turn_duration_count = 5000;

  if (command == 0){
    moveWheel(RIGHT_SERVO_CH, 0);
    moveWheel(LEFT_SERVO_CH, 0);
    counter = 0;
  } else if (command == 1) {
    moveWheel(RIGHT_SERVO_CH, -spd);
    moveWheel(LEFT_SERVO_CH, spd);
    counter++;
    if (counter > duration_count) {
      command = 0;
    }
  } else if (command == 2) {
    moveWheel(RIGHT_SERVO_CH, spd);
    moveWheel(LEFT_SERVO_CH, -spd);
    counter++;
    if (counter > duration_count) {
      command = 0;
    }
  } else if (command == 3) {
    moveWheel(RIGHT_SERVO_CH, spd);
    moveWheel(LEFT_SERVO_CH, spd);
    counter++;
    if (counter > turn_duration_count) {
      command = 0;
    }
  } else if (command == 4) {
    moveWheel(RIGHT_SERVO_CH, -spd);
    moveWheel(LEFT_SERVO_CH, -spd);
    counter++;
    if (counter > turn_duration_count) {
      command = 0;
    }
  }
}
