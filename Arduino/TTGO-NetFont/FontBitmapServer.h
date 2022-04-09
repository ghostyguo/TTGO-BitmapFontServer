/*
 * FontBitmapServer.h: get font bitmap from network service
 * 
 * by ghostyguy@gmail.com 2022/04/09
 * 
 */
#include <WiFi.h>

// Font Color 
// TTGO T-display: 16bit Font Color, R=5bit, G=6bit, B=5bit, setSwapBytes(true)
// https://learn.adafruit.com/adafruit-gfx-graphics-library/coordinate-system-and-units

#define FontBitmap_BLACK    0x0000
#define FontBitmap_BLUE     0x001F
#define FontBitmap_RED      0xF800
#define FontBitmap_GREEN    0x07E0
#define FontBitmap_CYAN     0x07FF
#define FontBitmap_MAGENTA  0xF81F
#define FontBitmap_YELLOW   0xFFE0 
#define FontBitmap_WHITE    0xFFFF

#define BitmapBufferSize 32400 //TFT ressolution 240*135 in TTGO T-display

class FontBitmapServer
{
  public:
    IPAddress IP=IPAddress(192,168,88,203);
    uint16_t Port=8080;
    unsigned short bitmap[BitmapBufferSize]; //Pre-Allocate maximun buffer
    unsigned short fontColor;
    int fontSize=16;
    int bitmapWidth=0, bitmapHeight=0;

    void begin();
    bool getBitmap(String text, int fontsize);

  private:
  
};

extern FontBitmapServer fontBitmapServer;
