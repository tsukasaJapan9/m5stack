#include <M5Stack.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define AUTH "AUTH"
#define SSID "SSID"
#define PASSWORD "PASS"

// command
int command = 0;
int counter = 0;

// for servo
#define RIGHT_SERVO_CH 0
#define LEFT_SERVO_CH 1

#define RIGHT_SERVO_PIN 21
#define LEFT_SERVO_PIN 22

/*=============================== */
/* setup */
/*=============================== */
void setup(void) {

  M5.begin();
  Serial.begin(115200);

  M5.Lcd.println("Wait for WiFi... ");

  Blynk.begin(AUTH, SSID, PASSWORD);

  M5.Lcd.println("WiFi connected");
  M5.Lcd.println("IP address: ");
  M5.Lcd.println(WiFi.localIP());

  // PWMの設定
  ledcSetup(RIGHT_SERVO_CH, 50, 10); // チャネル, 周波数=50Hz, 分解能=0-1023
  ledcAttachPin(RIGHT_SERVO_PIN, RIGHT_SERVO_CH); // ピン番号, チャネル

  ledcSetup(LEFT_SERVO_CH, 50, 10); // チャネル, 周波数=50Hz, 分解能=0-1023
  ledcAttachPin(LEFT_SERVO_PIN, LEFT_SERVO_CH); // ピン番号, チャネル
}

/*=============================== */
/* Blync */
/*=============================== */
BLYNK_WRITE(V0){
  command = 1;
  counter = 0;
}

BLYNK_WRITE(V1){
  command = 2;
  counter = 0;
}

BLYNK_WRITE(V2){
  command = 3;
  counter = 0;
}

BLYNK_WRITE(V3){
  command = 4;
  counter = 0;
}

/*=============================== */
/* servo control */
/*=============================== */
void moveServo(int channel, int angle) {
  ledcWrite(channel, map(angle, -90, 90, 26, 123));
}

void moveWheel(int channel, float spd) {
  ledcWrite(channel, map(spd, -100, 100, 1, 102));
}

/*=============================== */
/* main loop */
/*=============================== */
void loop() {
  Blynk.run();

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
