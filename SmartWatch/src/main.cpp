#include <M5Stack.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <time.h>

// ======== for ntp =========
#define JST     3600*9

// ======== for clock =========
static uint8_t conv2d(const char* p);

uint32_t targetTime = 0;
uint8_t hh = conv2d(__TIME__);
uint8_t mm = conv2d(__TIME__ + 3);
uint8_t ss = conv2d(__TIME__ + 6);

// ======== for wifi =========
WiFiMulti WiFiMulti;

// ======== for clock =========
#define TFT_GREY 0xBDF7
#define TFT_ROBOT 0x0082

/*=============================== */
/* setup */
/*=============================== */
int setLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return -1;
  }
  Serial.println(&timeinfo, "%Y %m %d %a %H:%M:%S");
  hh = timeinfo.tm_hour;
  mm = timeinfo.tm_min;
  ss = timeinfo.tm_sec;
  return 0;
}
void setup(void) {

  M5.begin();
  Serial.begin(115200);

  // wifi connetction
  WiFiMulti.addAP("SSID", "password");
  M5.Lcd.println("Wait for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      M5.Lcd.println(".");
      delay(500);
  }

  M5.Lcd.println("WiFi connected");
  M5.Lcd.println("IP address: ");
  M5.Lcd.println(WiFi.localIP());
  delay(500);

  //Draw the jpeg file form TF card
  M5.Lcd.setBrightness(200);
  M5.Lcd.drawJpgFile(SD, "/robot2.jpg");

  // ntp
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

  for (int i=0; i<20; i++) {
    int ret = setLocalTime();
    if (ret == 0) break;
    delay(100);
  }

  // for clock
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_GREY, TFT_ROBOT);
}

/*=============================== */
/* post data via http */
/*=============================== */
void post_data(int cmd1, int cmd2)
{
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient client;

    client.begin("http://192.168.1.16:40001/");
    client.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String post_data = "cmd1=";
    post_data.concat(String(cmd1));
    post_data.concat("&cmd2=");
    post_data.concat(String(cmd2));

    int rescode = client.POST(post_data);
    if(rescode > 0){
      String response = client.getString();
      Serial.println(rescode);
      Serial.println(response);
    }else{
      Serial.print("Error on sending POST: ");
      Serial.println(rescode);
    }
    client.end();
  }else{
    Serial.println("Error in WiFi connection");
  }
}

/*=============================== */
/* for clock */
/*=============================== */
// Function to extract numbers from compile time string
static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

void drawClock() {
  byte xcolon = 0, xsecs = 0;
  byte omm = 99, oss = 99;
  if (targetTime < millis()) {
    // Set next update for 1 second later
    targetTime = millis() + 1000;

    // Adjust the time values by adding 1 second
    ss++;              // Advance second
    if (ss == 60) {    // Check for roll-over
      ss = 0;          // Reset seconds to zero
      omm = mm;        // Save last minute time for display update
      mm++;            // Advance minute
      if (mm > 59) {   // Check for roll-over
        mm = 0;
        hh++;          // Advance hour
        if (hh > 23) { // Check for 24hr roll-over (could roll-over on 13)
          hh = 0;      // 0 for 24 hour clock, set to 1 for 12 hour clock
        }
      }
    }
    
    // Update digital time
    int xpos = 90;
    int ypos = 170 + 18; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 18;
    int fsize1 = 6;
    int fsize2 = 4;

    if (omm != mm) { // Redraw hours and minutes time every minute
      omm = mm;
      // Draw hours and minutes
      if (hh < 10) xpos += M5.Lcd.drawChar('0', xpos, ypos, fsize1); // Add hours leading zero for 24 hr clock
      xpos += M5.Lcd.drawNumber(hh, xpos, ypos, fsize1);             // Draw hours      
      xpos += M5.Lcd.drawChar(':', xpos, ypos - fsize1+5, fsize1);
      
      if (mm < 10) xpos += M5.Lcd.drawChar('0', xpos, ypos, fsize1); // Add minutes leading zero
      xpos += M5.Lcd.drawNumber(mm, xpos, ypos, fsize1);             // Draw minutes
      xsecs = xpos; // Sae seconds 'x' position for later display updates
    }
    
    if (oss != ss) { // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      //Draw seconds
      if (ss < 10) xpos += M5.Lcd.drawChar('0', xpos, ysecs, fsize2); // Add leading zero
      M5.Lcd.drawNumber(ss, xpos, ysecs, fsize2);                     // Draw seconds
    }
  }  
}

/*=============================== */
/* main loop */
/*=============================== */
void loop() {
  drawClock();

  M5.update();

  if (M5.BtnA.wasPressed()) {
    post_data(1, 0);
  }
  if (M5.BtnB.wasPressed()) {
    post_data(2, 0);
  }
}
