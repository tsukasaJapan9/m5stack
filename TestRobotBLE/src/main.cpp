#include <M5Stack.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// command
int command = 0;
int counter = 0;

// for servo
#define RIGHT_SERVO_CH 0
#define LEFT_SERVO_CH 1

#define RIGHT_SERVO_PIN 21
#define LEFT_SERVO_PIN 22

// for BLE
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

/*=============================== */
/* BLE */
/*=============================== */
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      M5.Lcd.println("connect");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      M5.Lcd.println("disconnect");
      deviceConnected = false;
    }
};

class Callbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    M5.Lcd.println("read");
    pCharacteristic->setValue("Hello World!");
  }

  void onWrite(BLECharacteristic *pCharacteristic) {
    M5.Lcd.println("write");
    std::string value = pCharacteristic->getValue();
    M5.Lcd.println(value.c_str());
    command = atoi(value.c_str());
  }
};

/*=============================== */
/* setup */
/*=============================== */
void setup(void) {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.println("BLE start.");

  // setup BLE
  BLEDevice::init("m5 robot");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  pCharacteristic->setCallbacks(new Callbacks());
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  // PWMの設定
  ledcSetup(RIGHT_SERVO_CH, 50, 10); // チャネル, 周波数=50Hz, 分解能=0-1023
  ledcAttachPin(RIGHT_SERVO_PIN, RIGHT_SERVO_CH); // ピン番号, チャネル

  ledcSetup(LEFT_SERVO_CH, 50, 10); // チャネル, 周波数=50Hz, 分解能=0-1023
  ledcAttachPin(LEFT_SERVO_PIN, LEFT_SERVO_CH); // ピン番号, チャネル
}

/*=============================== */
/* Blync */
/*=============================== */
// BLYNK_WRITE(V0){
//   command = 1;
//   counter = 0;
// }

// BLYNK_WRITE(V1){
//   command = 2;
//   counter = 0;
// }

// BLYNK_WRITE(V2){
//   command = 3;
//   counter = 0;
// }

// BLYNK_WRITE(V3){
//   command = 4;
//   counter = 0;
// }

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
