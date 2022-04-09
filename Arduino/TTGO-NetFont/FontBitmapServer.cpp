/*
 * FontBitmapServer.cpp: get font bitmap from network service
 * 
 * by ghostyguy@gmail.com 2022/04/09
 * 
 */

#include "FontBitmapServer.h"

//FontBitmapServer
FontBitmapServer fontBitmapServer;

//DebugLevel
#define debugLevel 0

//

//
// FontBitmap class
//

void FontBitmapServer::begin()
{
  fontColor = FontBitmap_RED;
}

bool FontBitmapServer::getBitmap(String text, int fontsize)
{
    WiFiClient client; 
  
    if (!client.connect(IP, Port)) {
        Serial.println("connect to font server fail");   
        client.stop();  
        return false;
    } 
    if (debugLevel>0)
      Serial.println("connect to font server ok");   
        
    // Send command to server
    String command="size="+String(fontsize) + "\ntext=" + text + "\n";    
    client.print(command);
    
    //wait for server response    
    #if (debugLevel>1)
      Serial.println("wait for server response");    
    #endif
      
    int timeout=300;
    while ((timeout>=0) && !client.available()) { 
      timeout--;
      delay(10);
      Serial.print(".");
    }
    if (timeout>=0) {      
      #if (debugLevel>1)
        Serial.println("OK");
      #endif
    }
    else
    {      
      Serial.println("server timeout");
      client.stop(); 
      return false;
    }

    int pos=0;
    int heightCount=0;
    bool notComplete=client.connected();
    bool isSuccess = true;
    
    bitmapWidth=0;
    bitmapHeight=0;    
    while (notComplete)
    {
      if (client.available())
      {
        if (bitmapWidth==0) //bitmapWidth is not defined
        {
          bitmapWidth = client.readStringUntil('\n').toInt();
          #if (debugLevel>0)
            Serial.print("bitmapWidth=");
            Serial.println(bitmapWidth);
          #endif
        }
        else if (bitmapHeight==0) //bitmapHeight is not defined
        {
          bitmapHeight=client.readStringUntil('\n').toInt(); 
          #if (debugLevel>0)
            Serial.print("bitmapHeight=");
            Serial.println(bitmapHeight);
          #endif
        } 
        else 
        {      
          String line = client.readStringUntil('\n');       
          #if (debugLevel>0)
            Serial.println(line);
          #endif  
          if (line.length()!=bitmapWidth) {
            Serial.println("font decode failed: line length error");
            notComplete = false;
            isSuccess = false;
          }
          for (int i=0; i<bitmapWidth; i++) {
            bitmap[pos] = (line[i]=='1') ? fontColor : FontBitmap_BLACK;
            if (pos<BitmapBufferSize) {
              pos++;
            }
            else
            {
              Serial.println("out of BitmapBufferSize");
              notComplete = false;
              isSuccess = false;                
            }
          }          
          if (++heightCount>=bitmapHeight) {
            notComplete = false;
          }
        }
      } //if (client.available())
    } //while (notComplete)          
    #if (debugLevel>0)
      Serial.println("Server disconnected"); 
    #endif  
    
    client.stop();
    return isSuccess;
}
