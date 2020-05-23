#include <M5Stack.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// for wifi
WiFiClient wifiClient;

// command
int command = 0;

// for servo
int RIGHT_SERVO_CH = 0;
int LEFT_SERVO_CH = 1;

int RIGHT_SERVO_PIN = 21;
int LEFT_SERVO_PIN = 22;

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
}

void connectToBroker() {
  M5.Lcd.println("trying to connect MQTT broker");
  mqttClient.setServer("192.168.11.9", 1883);
  mqttClient.setCallback(callback);
  mqttClient.setClient(wifiClient);
  if (!mqttClient.connect(THING_NAME)) {
    M5.Lcd.println(mqttClient.state());
  }
  mqttClient.subscribe("m5stack/control/#");
}

/*=============================== */
/* setup */
/*=============================== */
void setup(void) {

  M5.begin();
  Serial.begin(115200);

  // wifi connetction

  WiFi.begin("SSID", "password");
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

/*=============================== */
/* main loop */
/*=============================== */
void loop() {
  mqttClient.loop();

  if (command == 1) {
    moveServo(RIGHT_SERVO_CH, -90);
    moveServo(LEFT_SERVO_CH, -90);
  } else if (command == 2) {
    moveServo(RIGHT_SERVO_CH, 90);
    moveServo(LEFT_SERVO_CH, 90);
  }
}
