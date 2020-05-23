#include <M5Stack.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

// ======== for wifi =========
WiFiMulti WiFiMulti;

// ======== for clock =========
#define TFT_GREY 0xBDF7
#define TFT_ROBOT 0x0082

/*=============================== */
/* setup */
/*=============================== */
void setup(void) {

  M5.begin();
  Serial.begin(115200);

  // wifi connetction
  WiFiMulti.addAP("Buffalo-G-4EF0", "grtt4iss8yfn4");
  M5.Lcd.println("Wait for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      M5.Lcd.println(".");
      delay(500);
  }

  M5.Lcd.println("WiFi connected");
  M5.Lcd.println("IP address: ");
  M5.Lcd.println(WiFi.localIP());
  delay(500);

  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_GREY, TFT_ROBOT);

  // PWMの設定
  ledcSetup(0, 50, 10); // チャネル, 周波数=50Hz, 分解能=0-1023
  ledcAttachPin(21, 0); // ピン番号, チャネル

  ledcSetup(1, 50, 10); // チャネル, 周波数=50Hz, 分解能=0-1023
  ledcAttachPin(22, 1); // ピン番号, チャネル
}

/*=============================== */
/* main loop */
/*=============================== */
void loop() {
  ledcWrite(0, map(-90, -90, 90, 26, 123));
  ledcWrite(1, map(-90, -90, 90, 26, 123));
  delay(1000); 

  ledcWrite(0, map(90, -90, 90, 26, 123));
  ledcWrite(1, map(90, -90, 90, 26, 123));
  delay(1000); 
}
