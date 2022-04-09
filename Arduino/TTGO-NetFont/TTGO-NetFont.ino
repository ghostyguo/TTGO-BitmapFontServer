/*
 * TTGO-NetFont: get font bitmap from network service
 * 
 * by ghostyguy@gmail.com 2022/04/09
 * 
 */
 
//#include <WiFi.h>
#include <TFT_eSPI.h>
#include "FontBitmapServer.h"

// WiFi
const char *ssid = "Quake Insight";
const char *password = "oskay0729";


// TFT
TFT_eSPI tft = TFT_eSPI(); 

void setup()
{    
  Serial.begin(115200);

  // ----------
  //    TFT
  // ----------
  tft.begin();               // init TFT
  tft.setRotation(1);        // landscape
  tft.fillScreen(TFT_BLACK); // clear screen
  
  //https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts  
  tft.setFreeFont(&FreeSerif18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(35, 30);
  tft.printf("Font Server");
  
  tft.setFreeFont(&FreeSerif12pt7b);
  tft.setCursor(5, 60);
  tft.printf("ghostyguo@gmail.com");

  // ----------
  //    WiFi
  // ----------
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);    
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP:");  
  Serial.println(WiFi.localIP());
  
  // --------------------
  //    FontBitmapServer
  // --------------------
  
  tft.setSwapBytes(true);
  fontBitmapServer.begin();
}

void loop()
{
  String text = Serial.readString();
  if (text.length()==0) 
  {
    return;
  }
  if (text.substring(0, 5)=="size=") 
  {
      fontBitmapServer.fontSize = text.substring(5).toInt();
      Serial.print("new size=");
      Serial.println(fontBitmapServer.fontSize);
      return;
  } 
  if (text.substring(0, 6)=="color=") 
  {
      fontBitmapServer.fontColor = (unsigned)strtol(text.substring(6).c_str(), NULL, 16);
      Serial.print("new Color=");
      Serial.println(fontBitmapServer.fontColor, HEX);
      return;
  } 
  unsigned long t0=millis(); //performance counter
  if (fontBitmapServer.getBitmap(text, fontBitmapServer.fontSize)) 
  {
    unsigned long t1=millis(); //performance counter
    // 240*135
    tft.fillRect(0, 70, 240, 135-90, TFT_BLACK ); //clear text area
    tft.pushImage(5, 70, fontBitmapServer.bitmapWidth, fontBitmapServer.bitmapHeight, fontBitmapServer.bitmap);  //show font bitmap    
    unsigned long t2=millis(); //performance counter
    
    //performance measurement
    Serial.print("get=");
    Serial.print(t1-t0);
    Serial.print("ms, paint=");
    Serial.print(t2-t1);
    Serial.println("ms");
  }
  else {
    Serial.println("Paint ???");
    tft.fillRect(0, 70, 240, 135-90, TFT_BLACK ); //clear text area
    tft.setFreeFont(&FreeSerif18pt7b);
    tft.setCursor(5, 90);
    tft.printf("???");
  }
}
