#include <stdint.h>
//#include <avr/pgmspace.h>

#include "settings.h"
#include "settings_eeprom.h"
#include "settings_internal.h"
#include "ui.h"
#include "bitmaps.h"
#include "receiver_spi.h"
#include "receiver.h"

#include "TTVout.h"
TTVout TV;

#define OLED_RESET -1

namespace Ui {
    #ifdef OLED_128x64_ADAFRUIT_SCREENS
      Adafruit_SSD1306 display(OLED_RESET);      
    #endif 
    
    bool shouldDrawUpdate = false;
    bool shouldDisplay = false;
    bool shouldFullRedraw = false;
    bool isTvOn = false;

    void setup() {
        #ifdef OLED_128x64_ADAFRUIT_SCREENS  

          Wire.setClock(400000);
          
          display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setTextWrap(false);  
          display.clearDisplay(); 
          needDisplay();
          update(); 
        #endif
        
//        TV.begin(SC_256x192); // tone will not work until begin is called.
        TV.begin(SC_448x108);
        TV.end();    
    }

    void tvOn() {

        ReceiverSpi::setPowerDownRegister(0b01010000110000010011);
        
        display.ssd1306_command(SSD1306_DISPLAYOFF);  
        
//        TV.begin(SC_256x192);
        TV.begin(SC_448x108);

        Ui::sdToTtvout(); 
        
        isTvOn = true;
    }

    void tvOff() { 

        ReceiverSpi::setPowerDownRegister(0b00010000110000010011);

        Receiver::setChannel(Receiver::activeChannel);
        
        TV.end();    
        
        if (EepromSettings.useOledScreen) {
          display.ssd1306_command(SSD1306_DISPLAYON);
          display.display();          
        }
         
        isTvOn = false;
    }

    // TODO replace with buffer from OLED
    void sdToTtvout() {      
      for(int y=0; y<64; y++) {
        for(int x=0; x<128; x++) {            
          bool colour = display.getPixel(x,y);
//          TV.draw_rect(x*2, y*3, 2, 3, colour, colour);
          TV.draw_rect(32+x*3, 22+y, 3, 1, colour, colour);         
        }
      }      
    }

    void beep() { 
        uint16_t freq = 5000; // frequency in Hz
        TV.tone(freq, BEEPER_CHIRP);
    }
    void beep(uint16_t freq) { 
        TV.tone(freq, BEEPER_CHIRP);
    }

    void update() {
        if (shouldDisplay) {
              #ifdef OLED_128x64_ADAFRUIT_SCREENS
                      
                  if (isTvOn) {
                    Ui::sdToTtvout();
                  } else {        
                    if (EepromSettings.useOledScreen) { 
                      display.display();          
                    } else {
                      display.ssd1306_command(SSD1306_DISPLAYOFF);
                    }
                  }
                  
              #endif 
              #ifdef TVOUT_SCREENS
              #endif
            shouldDisplay = false;
        }
    }

    void drawGraph(
        const uint8_t data[],
        const uint8_t dataSize,
        const uint8_t dataScale,
        const uint8_t x,
        const uint8_t y,
        const uint8_t w,
        const uint8_t h
    ) {
        #define SCALE_DATAPOINT(p) (p * (float)h / (float)dataScale)
        #define CLAMP_DATAPOINT(p) (p > (float)dataScale) ? (float)dataScale : ((p < 0) ? 0 : p);

        Ui::clearRect(x, y, w, h);
        
        const float xScaler = (float)w / ((float)dataSize - 1);
        const float xScalarMissing = (float)w - (xScaler * ((float)dataSize - 1));

        float xNext = (float)x;

        for (uint8_t i = 0; i < dataSize - 1; i++) {
            const float dataPoint = CLAMP_DATAPOINT((float)data[i]);
            const float dataPointNext = CLAMP_DATAPOINT((float)data[i + 1]);

            // Need to invert the heights so it shows the right way on the screen.
            const float dataPointHeight = (float)h - SCALE_DATAPOINT((float)dataPoint);
            const float dataPointNextHeight = (float)h - SCALE_DATAPOINT((float)dataPointNext);

            const float xEnd = xNext + xScaler + (i == 0 || i == dataSize - 2 ? (xScalarMissing + 1) / 2 : 0);
            
              #ifdef OLED_128x64_ADAFRUIT_SCREENS
                display.drawLine(
                    xNext,
                    y + dataPointHeight,
                    xEnd,
                    y + dataPointNextHeight,
                    WHITE
                );
              #endif
              #ifdef TVOUT_SCREENS
                display.draw_line(
                    xNext,
                    y + dataPointHeight,
                    xEnd,
                    y + dataPointNextHeight,
                    WHITE
                );
              #endif
            xNext = xEnd;
        }

        #undef SCALE_DATAPOINT
        #undef CLAMP_DATAPOINT
    }

    void drawSolidGraph(
        const uint8_t data[],
        const uint8_t dataSize,
        const uint8_t dataScale,
        const uint8_t x,
        const uint8_t y,
        const uint8_t w,
        const uint8_t h
    ) {
        #define SCALE_DATAPOINT(p) (p * (float)h / (float)dataScale)
        #define CLAMP_DATAPOINT(p) (p > (float)dataScale) ? (float)dataScale : ((p < 0) ? 0 : p);
        
        const float xScaler = (float)w / ((float)dataSize - 1);
        const float xScalarMissing = (float)w - (xScaler * ((float)dataSize - 1));

        float xNext = (float)x;

        for (uint8_t i = 0; i < dataSize - 1; i++) {
            const float dataPoint = CLAMP_DATAPOINT((float)data[i]);
            const float dataPointNext = CLAMP_DATAPOINT((float)data[i + 1]);

            // Need to invert the heights so it shows the right way on the screen.
            const float dataPointHeight = (float)h - SCALE_DATAPOINT((float)dataPoint);
            const float dataPointNextHeight = (float)h - SCALE_DATAPOINT((float)dataPointNext);

            const float xEnd = xNext + xScaler + (i == 0 || i == dataSize - 2 ? (xScalarMissing + 1) / 2 : 0);

            const float m = (dataPointNextHeight - dataPointHeight) / (xEnd - xNext);
            
            for (float j = xNext; j < xEnd; j=j+1) {
              float fillPointHeight = dataPointHeight + (j - xNext) * m;
              if (fillPointHeight < 0) {
                fillPointHeight = 0;
                }
              drawFastVLine((int)j, y, (int)fillPointHeight, BLACK);
              drawFastVLine((int)j, (int)fillPointHeight, y + h - (int)fillPointHeight, WHITE);
            }
            
            xNext = xEnd;
        }

        #undef SCALE_DATAPOINT
        #undef CLAMP_DATAPOINT
    }


    void drawDashedHLine(
        const int x,
        const int y,
        const int w,
        const int step
    ) {
        for (int i = 0; i < w; i += step) {
          #ifdef OLED_128x64_ADAFRUIT_SCREENS
            display.drawFastHLine(x + i, y, step / 2, WHITE);
          #endif
          #ifdef TVOUT_SCREENS
            display.draw_row(y, x + i, x + i + (step / 2), WHITE);
          #endif
        }
    }

    void drawDashedVLine(
        const int x,
        const int y,
        const int h,
        const int step
    ) {
        for (int i = 0; i < h; i += step) {
          #ifdef OLED_128x64_ADAFRUIT_SCREENS
            display.drawFastVLine(x, y + i, step / 2, INVERSE);
          #endif
          #ifdef TVOUT_SCREENS
            display.draw_column(x, y + i, y + i + (step / 2), WHITE);
          #endif
        }
    }

    void drawFastHLine(
        const int x,
        const int y,
        const int w,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.drawFastHLine(x, y, w, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_row(y, x, x+w, color);
      #endif
    }

    void drawFastVLine(
        const int x,
        const int y,
        const int h,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.drawFastVLine(x, y, h, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_column(x, y, y+h, color);
      #endif
    }

    void drawRoundRect(
        const int x,
        const int y,
        const int w,
        const int h,
        const int r,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.drawRoundRect(x, y, w, h, r, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_rect(x, y, w, h, color);
      #endif
    }

    void fillRect(
        const int x,
        const int y,
        const int w,
        const int h,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.fillRect(x, y, w, h, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_rect(x, y, w, h, color, color);
      #endif
    }

    void fillTriangle(
        const int x0,
        const int y0,
        const int x1,
        const int y1,
        const int x2,
        const int y2,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.fillTriangle(
          x0, y0, x1, y1, x2, y2, color
        );
      #endif
      #ifdef TVOUT_SCREENS
        for (int x=x0; x<=x2; x++) {
          display.draw_line(x,y0,x1,y1,color);
        }
      #endif
    }

    void drawBitmap(const int x, const int y, const unsigned char *image, const int w, const int h, const int color) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS

        // draw Bitmap not working.  On the TODO list and remove the below work around.
        int setColor;
        for (int i=0; i<w; i++) {
          for (int j=0; j<h; j++) {
            setColor = bitRead( image[(j*w+i) / 8], 7 - (j*w+i) % 8 );
            if (color) {
              display.drawPixel(x+i, y+j, setColor);
            } else {
              display.drawPixel(x+i, y+j, !setColor);
            }
          }
        }
      #endif
      #ifdef TVOUT_SCREENS
        display.bitmap(x, y, image, 0, w, h);
        if (color == BLACK) {
          display.draw_rect(x, y, w, h,INVERT,INVERT);
        }
      #endif
    }

    void drawCircle(const int x, const int y,const int r, const int color, const int fillcolor) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.drawCircle(x, y, r, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_circle(x, y, r, color, fillcolor);
      #endif
    }

    void drawBullseye(const int x, const int y,const int r, const int color) {
      drawCircle(x, y, r-2, color, BLACK);
      drawFastHLine(x, y, 0, color);
      drawFastHLine(x-r, y, 2*r+1, color);
      drawFastVLine(x, y-r, 2*r+1, color);
    }
    
    void setTextColor(const int color) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.setTextColor(color);
      #endif
      #ifdef TVOUT_SCREENS
        // No equivalent function
      #endif
    }
    
    void setTextSize(const int size) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.setTextSize(size); 
      #endif
      #ifdef TVOUT_SCREENS
      //TODO add larger font
        if (size == 1) {
          display.select_font(font4x6);
        }
        else if (size == 2) {
          display.select_font(font6x8);
        }
        else if (size == 3) {
          display.select_font(font8x8);
        }
        else if (size == 4) {
          display.select_font(font8x8);
        }
        else if (size == 5) {
          display.select_font(font8x8);
        }
        else if (size == 6) {
          display.select_font(font8x8);
        }
        else {
          display.select_font(font4x6);
        }        
      #endif
    }
    
    void setCursor(const int x, const int y) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.setCursor(x,y);
      #endif
      #ifdef TVOUT_SCREENS
        display.set_cursor(x,y);
      #endif
    }

    // TODO move print here
    void print(const char text) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
//        display.print(text);
      #endif
      #ifdef TVOUT_SCREENS
//        display.print(text);
      #endif
    }
          
    void clear() {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.clearDisplay();
      #endif
      #ifdef TVOUT_SCREENS
        display.clear_screen();
      #endif
    }

    void clearRect(const int x, const int y, const int w, const int h) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.fillRect(x, y, w, h, BLACK);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_rect(x, y, w, h,BLACK,BLACK);
      #endif
    }
          
    void needUpdate() {
        shouldDrawUpdate = true;
    }

    void needDisplay() {
        shouldDisplay = true;
    }

    void needFullRedraw() {
        shouldFullRedraw = true;
    }
}


