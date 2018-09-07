/*
 * Setings file by Shea Ivey

The MIT License (MIT)

Copyright (c) 2015 Shea Ivey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

// === Module ==================================================================
//
// You can choose one module only
//
// =============================================================================

#define FENIX_QUADVERSITY

//#define REALACC_RX5808_PRO_PLUS_OSD

// === Display Module ==========================================================
//
// You can choose one display module only. Only 128x64 OLED displays are
// supported.
//
// =============================================================================

#define OLED_128x64_ADAFRUIT_SCREENS

// Enable this if your screen is upside down.
//#define USE_FLIP_SCREEN

#define OLED_ADDRESS 0x3C // I2C address for display (0x3C or 0x3D, usually)

// === Receiver Module =========================================================
//
// Select which receiever you are using. Required for time tuning.
//
// =============================================================================

#define RX5808
//#define RX5880

// Can enable this to powerdown the audio blocks on the RX58xx if you don't
// need it. Save a tiny bit of power, make your videos less noisy.
//
// WARNING: Makes RSSI act a little weird.
//#define DISABLE_AUDIO

// === Features ===============================================================

// You can use any of the arduino analog pins to measure the voltage of the
// battery. See additional configuration below.
//#ifdef FENIX_QUADVERSITY
//    #define USE_VOLTAGE_MONITORING
//#endif

// === Pins ====================================================================

#ifdef FENIX_QUADVERSITY

    #define PIN_BUTTON_UP_PRESSED PA12 // 33
    #define PIN_BUTTON_MODE_PRESSED PA15 // 38
    #define PIN_BUTTON_DOWN_PRESSED PA8 // 29
    #define PIN_BUTTON_RIGHT_PRESSED PA11 // 32
    #define PIN_BUTTON_LEFT_PRESSED PB15 // 28
    
    #define PIN_BUTTON_FATSHARK_EB0 PB3 // 39
    #define PIN_BUTTON_FATSHARK_EB1 PB4  // 40
    #define PIN_BUTTON_FATSHARK_EB2 PB5  // 41
    
    #define PIN_LED PB8 // 45
    #define PIN_BUZZER PB9 // 46 // Must be an active buzzer, not passive.
    #define PIN_OSDCONTROL PC15 // 4
    
    #define PIN_SPI_DATA PB12          // 25 // CH1
    #define PIN_SPI_SLAVE_SELECT PB13  // 26 // CH2
    #define PIN_SPI_CLOCK PB14         // 27 // CH3
    
    #define PIN_RSSI_A PA3 // 13
    #define PIN_LED_A PB1 // 19
    #define PIN_RSSI_B PA2 // 12
    #define PIN_LED_B PB0 // 18
    #define PIN_RSSI_C PA6 // 16
    #define PIN_LED_C PC14 // 3
    #define PIN_RSSI_D PA4 // 15
    #define PIN_LED_D PC13 // 2
    
    #define PIN_VBAT PA0 // 10
    
    //OLED_SCL PB6
    //OLED_SDA PB7
    //
    // This pins are fixed in the library and Im unsure how to change them.
    //TVout PA1 
    //TVout PA7
#endif

// === Pins ====================================================================

#ifdef REALACC_RX5808_PRO_PLUS_OSD
// Buttons (required)
    #define PIN_BUTTON_UP_PRESSED PA8     // 29
    #define PIN_BUTTON_MODE_PRESSED PB14  // 27
    #define PIN_BUTTON_DOWN_PRESSED PB12  // 25
    
    #define PIN_BUTTON_FATSHARK_EB0 PB13  // 26
    #define PIN_BUTTON_FATSHARK_EB1 PA11  // 32
//    #define PIN_BUTTON_FATSHARK_EB2 PB15  // 28 ??? 
    
    #define PIN_BUZZER PB8                // 45 - Must be an active buzzer, not passive.
    
    #define PIN_SPI_DATA PA5              // 15 //CH1 
    #define PIN_SPI_SLAVE_SELECT_A PA4    // 14 //CH2 
    #define PIN_SPI_SLAVE_SELECT_B PA3    // 13 //CH2  
    #define PIN_SPI_CLOCK PA7             // 17 //CH3 

    #define TS5A3159 PC15           // 4
    
    #define PIN_RSSI_A PA0          // 10
    #define PIN_LED_A PB9           // 46
    #define PIN_RSSI_B PA1      // 11
    #define PIN_LED_B PA2       // 12
    
    #define PIN_RSSI_C PA0 // dummy pin
    #define PIN_LED_C PB9 // dummy pin
    #define PIN_RSSI_D PA1 // dummy pin
    #define PIN_LED_D PB9 // dummy pin
    
    #define PIN_VBAT PA6 // dummy pin
    
    //OLED_SCL PB6
    //OLED_SDA PB7
    //
    // This pins are fixed in the library and Im unsure how to change them.
    //TVout PA9  // 30
    //TVout PB15 // 28

#endif

// === Diversity ===============================================================

// RSSI strength should be greater than the value below (percent) over the
// other receiver before we switch. This pervents flicker when RSSI values
// are close and delays diversity checks counter.
//#define DIVERSITY_HYSTERESIS 2

// How long (ms) the RSSI strength has to have a greater difference than the
// above before switching.
//#define DIVERSITY_HYSTERESIS_PERIOD 5


// === Voltage Monitoring ======================================================

// You can use any Arduino analog input to measure battery voltage. Keep in
// mind that A4 and A5 is used by OLED and A6 and A7 are used for measuring
// RSSI.
//
// Use a voltage divider to lower the voltage to max 5V - values for max 13V
// (3s). You can use a 100nF capacitor near the Arduino pin to smooth the
// voltage.
//
//           R1 = 5.6k
//    BAT+ ----====----+----+---- ARDUINO ANALOG PIN
//                     |    |
//                     |    |  (optional)
//                     |    || 100n CAP
//                     |    |
//           R2 = 3.3k |    |
//    BAT- ----====----|----|

// Voltage levels
#define WARNING_VOLTAGE 72 // 3.6V per cell for 2S
#define CRITICAL_VOLTAGE 66 // 3.3V per cell for 2S
#define VBAT_SCALE 110
#define VBAT_OFFSET 0

// Alarm sounds
#define ALARM_EVERY_SEC 20000
#define CRITICAL_BEEP_EVERY_MSEC 400
#define CRITICAL_BEEPS 3
#define WARNING_BEEP_EVERY_MSEC 200
#define WARNING_BEEPS 2

// === RSSI ====================================================================

// RSSI default raw range.
#define RSSI_MIN_VAL 500
#define RSSI_MAX_VAL 2000

// 75% threshold, when channel is printed in spectrum.
#define RSSI_SEEK_FOUND 75

// 80% under max value for RSSI.
//#define RSSI_SEEK_TRESHOLD 50

// Scan loops for setup run.
#define RSSI_SETUP_RUN 1 //3

// === Misc ====================================================================

// Key debounce delay in milliseconds.
// Good values are in the range of 100-200ms.
// Shorter values will make it more reactive, but may lead to double trigger.
#define BUTTON_DEBOUNCE_DELAY 50

#define SCREENSAVER_TIMEOUT 30 // Seconds to wait before entering screensaver
#define SCREENSAVER_DISPLAY_CYCLE 3 // Seconds between switching logo/channel

// Time needed to hold mode to get to menu
#define BUTTON_WAIT_FOR_MENU 1000

#define BEEPER_CHIRP 25

#endif // file_defined
