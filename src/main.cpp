/*
 * ที่แอพ Blynk ให้เพิ่มโปรเจค และเพิ่ม ปุ่ม 1 ปุ่ม โดยใช้ V0
 */

#define BLYNK_TEMPLATE_ID               "TMPxxxxxx"
#define BLYNK_TEMPLATE_NAME             "Device"
#define BLYNK_FIRMWARE_VERSION          "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#define APP_DEBUG


#include <BlynkGOv3.h>
#include "src/BlynkEdgent/BlynkEdgent.h"

GButton btn_wificonfig;
GLabel  lb_wificonfig;

GRect  rect_blynk_connection;
  GLabel lb_blynk_connection("B", rect_blynk_connection);

GLabel lb_clock;
GLed   led;

void setup()
{
  Serial.begin(9600); Serial.println();
  BlynkGO.begin();
  
  btn_wificonfig = SYMBOL_WIFI;
  btn_wificonfig.round_design();
  btn_wificonfig.label().color(TFT_RED);
  btn_wificonfig.align(ALIGN_TOP_RIGHT,-5);
  btn_wificonfig.onClicked([](GWidget*widget){
    static SoftTimer timer;
    static int8_t cnt;
    cnt = 180;
    BlynkState::set(MODE_CONFIGURING);
    enterConfigMode();
    Serial.println("[WiFi] configurating...");
    lb_wificonfig.show(true);
    BlynkGO.update();
    timer.setInterval(500,[cnt,timer](){
      btn_wificonfig.label().opa( cnt%2? 255 : 0 );
      if(--cnt <=0){
        btn_wificonfig.label().opa(255);
        BlynkState::set(MODE_CONNECTING_NET);
        lb_wificonfig.hidden(true);
        timer.del();
      }
    });
  });
  lb_wificonfig.font(prasanmit_20, TFT_SILVER);
  lb_wificonfig.align(ALIGN_BOTTOM);
  lb_wificonfig = "ตั้งค่า WiFi จาก http://192.168.4.1/";
  lb_wificonfig.hidden(true);

  rect_blynk_connection.size(30,30);
  rect_blynk_connection.corner_radius(5);
  rect_blynk_connection.color(TFT_RED);
  rect_blynk_connection.align(btn_wificonfig, ALIGN_LEFT,-3);
    lb_blynk_connection = "B";

  lb_clock.align(rect_blynk_connection, ALIGN_LEFT,-10);

  led.size(100,100);
  led.color(TFT_BLUE);
  led.OFF();
  led.onClicked([](GWIDGET){
    led.isON()? led.OFF(): led.ON();
    BlynkGO.flashMem("LED", led.isON());  // บันทึกสถานะลง flashMem
    Blynk.virtualWrite(V0, led.isON() );  // อัพเดตไปให้ แอพ Blynk หลังจากผู้ใช้กด led ที่ จอ BeeNeXT
  });
  BlynkGO.update();

  //-------------------------
  BlynkEdgent.begin();
}

void loop() {
  BlynkGO.update();
  BlynkEdgent.run();
}

WIFI_CONNECTED(){
  btn_wificonfig.label().color(TFT_WHITE);
}

WIFI_DISCONNECTED(){
  btn_wificonfig.label().color(TFT_RED);
}

NTP_SYNCED(){
  static SoftTimer timer;
  timer.setInterval(1000,[](){
    lb_clock = StringX::printf("%02d:%02d:%02d", hour(), minute(), second());
  });
}

BLYNK_CONNECTED(){
  rect_blynk_connection.color(TFT_COLOR_HSV(120,100,70));
  Blynk.virtualWrite(V0, (bool) BlynkGO.flashMem_Int("LED"));  // update ไปให้ แอพ Blynk หลังเชื่อมต่อสำเร็จ ด้วย ค่าที่บันทึกจาก flashMem
}

BLYNK_DISCONNECTED(){
  rect_blynk_connection.color(TFT_RED);
}

// รับค่าจาก แอพ Blynk ผ่านทาง V0
BLYNK_WRITE(V0){
  (param.asInt() > 0)? led.ON() : led.OFF();
  BlynkGO.flashMem("LED", led.isON());  // บันทึกสถานะลง flashMem
}

