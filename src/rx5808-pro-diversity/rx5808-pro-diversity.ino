/*
   SPI driver based on fs_skyrf_58g-main.c Written by Simon Chambers
   TVOUT by Myles Metzel
   Scanner by Johan Hermen
   Inital 2 Button version by Peter (pete1990)
   Refactored and GUI reworked by Marko Hoepken
   Universal version my Marko Hoepken
   Diversity Receiver Mode and GUI improvements by Shea Ivey
   OLED Version by Shea Ivey
   Seperating display concerns by Shea Ivey

  The MIT License (MIT)

  Copyright (c) 2015 Marko Hoepken

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

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"
#include "pstr_helper.h"

#include "channels.h"
#include "receiver.h"
#include "receiver_spi.h"
#include "buttons.h"
#include "state.h"
#include "state_menu.h"
#include "ui.h"
#include "bitmaps.h"
#include "osd_switching.h"
#include "voltage.h"

static void globalMenuButtonHandler(
  Button button,
  Buttons::PressType pressType
);

uint16_t currentTime = 0;
uint16_t lastTime = 0;  
  
void setup()
{
  Serial.begin(9600); 
  
  EepromSettings.load();

  setupPins();

  StateMachine::setup();
  Ui::setup(); 

  if (!EepromSettings.useFastBoot) {
    
    Ui::clear();      
  
  // Boot Logo avaibale as an option
  //  Ui::drawBitmap(
  //      0,
  //      0,
  //      bootlogo,
  //      SCREEN_WIDTH,
  //      SCREEN_HEIGHT,
  //      WHITE
  //  );  
  
    Ui::setTextColor(WHITE);
    
    Ui::setTextSize(1);
    Ui::setCursor(0, 0);       
    Ui::display.print(PSTR2("Booting...")); 
    
    Ui::setTextSize(4);
    Ui::setCursor(4, 18);       
    Ui::display.print(PSTR2("FENIX")); 
  
    Ui::fillRect(3, 51, 118, 9, WHITE);
    Ui::setTextColor(BLACK);
    Ui::setTextSize(1);
    Ui::setCursor(5, 52);       
    Ui::display.print(PSTR2("QUADVERSITY    v0.1"));   
    
    Ui::needDisplay();
    Ui::update();  
  }

  Buttons::registerChangeFunc(globalMenuButtonHandler);

  // Flash lights as startup sequency
  for (int x=0; x<20; x++) {
    digitalWrite(PIN_LED_A,!digitalRead(PIN_LED_A));
    digitalWrite(PIN_LED_B,!digitalRead(PIN_LED_B));
    #ifdef FENIX_QUADVERSITY
      digitalWrite(PIN_LED_C,!digitalRead(PIN_LED_C));
      digitalWrite(PIN_LED_D,!digitalRead(PIN_LED_D));
      digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    #endif
    Ui::beep(x*500);
  }
  
  if (!EepromSettings.useFastBoot) {
    while (millis() < 2000) { // Delay to show boot screen
      delay(5);
    }
  }

//   Switch to initial state.
  StateMachine::switchState(EepromSettings.lastKnownState);  
  if (!EepromSettings.isCalibrated) {
      StateMachine::switchState(StateMachine::State::SETTINGS_RSSI);
  }

  // Setup complete.
  #ifdef FENIX_QUADVERSITY
    digitalWrite(PIN_LED, HIGH);  // ON
  #endif

  // Has to be last setup() otherwise channel may not be set.
  // RX possibly not botting quick enough if setup() is called earler.
  Receiver::setup(); 
}

void setupPins() {

  #ifdef FENIX_QUADVERSITY
    // Use to allow use of PB3 as a button
    afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY); // http://www.stm32duino.com/viewtopic.php?t=1130#p13918
  #endif
  
    pinMode(PIN_BUTTON_UP_PRESSED, INPUT_PULLUP);
    pinMode(PIN_BUTTON_MODE_PRESSED, INPUT_PULLUP);
    pinMode(PIN_BUTTON_DOWN_PRESSED, INPUT_PULLUP);
    pinMode(PIN_BUTTON_FATSHARK_EB0, INPUT_PULLUP);
    pinMode(PIN_BUTTON_FATSHARK_EB1, INPUT_PULLUP);
  #ifdef FENIX_QUADVERSITY
    pinMode(PIN_BUTTON_RIGHT_PRESSED, INPUT_PULLUP);
    pinMode(PIN_BUTTON_LEFT_PRESSED, INPUT_PULLUP);
    pinMode(PIN_BUTTON_FATSHARK_EB2, INPUT_PULLUP); 
  #endif
  
  #ifdef FENIX_QUADVERSITY
    pinMode(PIN_OSDCONTROL, OUTPUT);
    digitalWrite(PIN_OSDCONTROL, HIGH);
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
  #endif
  
  #ifdef REALACC_RX5808_PRO_PLUS_OSD
    pinMode(TS5A3159, OUTPUT);
  #endif
  
    pinMode(PIN_LED_A, OUTPUT);
    digitalWrite(PIN_LED_A, LOW);
    pinMode(PIN_LED_B, OUTPUT);
    digitalWrite(PIN_LED_B, LOW);
    pinMode(PIN_LED_C, OUTPUT);
    digitalWrite(PIN_LED_C, LOW);
    pinMode(PIN_LED_D, OUTPUT);
    digitalWrite(PIN_LED_D, LOW);
  
    pinMode(PIN_RSSI_A, INPUT_ANALOG);
    pinMode(PIN_RSSI_B, INPUT_ANALOG);
    pinMode(PIN_RSSI_C, INPUT_ANALOG);
    pinMode(PIN_RSSI_D, INPUT_ANALOG);
  
  #ifdef FENIX_QUADVERSITY
    pinMode(PIN_SPI_SLAVE_SELECT, OUTPUT);
  #endif
  #ifdef REALACC_RX5808_PRO_PLUS_OSD
    pinMode(PIN_SPI_SLAVE_SELECT_A, OUTPUT);
    pinMode(PIN_SPI_SLAVE_SELECT_B, OUTPUT);
  #endif
    pinMode(PIN_SPI_DATA, OUTPUT);
    pinMode(PIN_SPI_CLOCK, OUTPUT);
    
  #ifdef FENIX_QUADVERSITY
    digitalWrite(PIN_SPI_SLAVE_SELECT, HIGH);
  #endif
  #ifdef REALACC_RX5808_PRO_PLUS_OSD
    digitalWrite(PIN_SPI_SLAVE_SELECT_A, HIGH);
    digitalWrite(PIN_SPI_SLAVE_SELECT_B, HIGH);
  #endif
    digitalWrite(PIN_SPI_CLOCK, LOW);
    digitalWrite(PIN_SPI_DATA, LOW);
    
  #ifdef FENIX_QUADVERSITY
    pinMode(PIN_VBAT, INPUT_ANALOG);
  #endif
}

void loop() {

  currentTime = millis();
  Serial.println(currentTime - lastTime);
  lastTime = currentTime;
  
  Buttons::update();
  Receiver::update();
  #ifdef FENIX_QUADVERSITY
    Voltage::update();
  #endif
  if (EepromSettings.useOledScreen || Ui::isTvOn) {
      StateMachine::update();
      Ui::update();
      EepromSettings.update();
  }
  
  if (EepromSettings.saveScreenOn) {
    if (
      StateMachine::currentState != StateMachine::State::SCREENSAVER
      && StateMachine::currentState != StateMachine::State::BANDSCAN
      && StateMachine::currentState != StateMachine::State::FINDER
      && (millis() - Buttons::lastChangeTime) >
      (SCREENSAVER_TIMEOUT * 1000)
    ) {
      StateMachine::switchState(StateMachine::State::SCREENSAVER);
    }    
  }
}


static void globalMenuButtonHandler(
  Button button,
  Buttons::PressType pressType
) {
  
  if (
    ( button == Button::MODE_PRESSED || button == Button::UP_PRESSED ||button == Button::DOWN_PRESSED || button == Button::RIGHT_PRESSED || button == Button::LEFT_PRESSED
      || button == Button::FATSHARK_EB0 || button == Button::FATSHARK_EB1 || button == Button::FATSHARK_EB2)
    && 
    (pressType == Buttons::PressType::SHORT || pressType == Buttons::PressType::LONG || pressType == Buttons::PressType::HOLDING) 
    &&
    EepromSettings.buttonBeep
  ) {
     Ui::beep();
  }
  
  if (
    StateMachine::currentState != StateMachine::State::MENU &&
    button == Button::MODE_PRESSED &&
    pressType == Buttons::PressType::HOLDING
  ) {
    StateMachine::switchState(StateMachine::State::MENU);
  }
  
  if ( button == Button::DOWN_PRESSED && pressType == Buttons::PressType::LONG ) {
    switchOSDOutputState();
  }
  
}
