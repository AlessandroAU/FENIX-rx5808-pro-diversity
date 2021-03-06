#include <stddef.h>

#include "state.h"

#include "state_home.h"
#include "state_home_simple.h"
#include "state_home_stats.h"
#include "state_screensaver.h"
#include "state_search.h"
#include "state_bandscan.h"
#include "state_menu.h"
#include "state_settings.h"
#include "state_settings_internal.h"
#include "state_settings_rssi.h"
#include "state_spectator.h"
#include "state_favourites.h"
#include "state_finder.h"
#include "state_custom_logo.h"
#include "state_laptimer.h"

#include "ui.h"
#include "buttons.h"
#include "settings_eeprom.h"
#include "timer.h"


void *operator new(size_t size, void *ptr){
  return ptr;
}

#define MAX(a, b) (a > b ? a : b)

#define STATE_BUFFER_SIZE \
    MAX(sizeof(HomeStateHandler), \
    MAX(sizeof(HomeSimpleStateHandler), \
    MAX(sizeof(HomeStatsStateHandler), \
    MAX(sizeof(ScreensaverStateHandler), \
    MAX(sizeof(SearchStateHandler), \
    MAX(sizeof(BandScanStateHandler), \
    MAX(sizeof(MenuStateHandler), \
    MAX(sizeof(SettingsStateHandler), \
    MAX(sizeof(SettingsInternalStateHandler), \
    MAX(sizeof(SettingsRssiStateHandler), \
    MAX(sizeof(SpectatorStateHandler), \
    MAX(sizeof(FavouritesStateHandler), \
    MAX(sizeof(FinderStateHandler), \
    MAX(sizeof(LaptimerStateHandler), \
        sizeof(CustomLogoStateHandler) \        
    ))))))))))))))
;

namespace StateMachine {
    static void onButtonChange(Button button, Buttons::PressType pressType);
    static StateHandler *getStateHandler(State stateType);


    static uint8_t stateBuffer[STATE_BUFFER_SIZE];
    static StateHandler* currentHandler = nullptr;
    State currentState = State::BOOT;
    State lastState = currentState;


    void setup() {
        Buttons::registerChangeFunc(onButtonChange);
    }

    void update() {
  
        if (currentHandler) {
            currentHandler->onUpdate();

            // FIXME: This should probably be handled in the UI module but not
            // 100% on how to decouple them at this stage
            static Timer drawTimer = Timer(OLED_FRAMERATE);
            if (currentHandler
                && Ui::shouldDrawUpdate
                && drawTimer.hasTicked()
            ) {
                if (Ui::shouldFullRedraw) {
                    currentHandler->onInitialDraw();
                    Ui::shouldFullRedraw = false;
                }

                currentHandler->onUpdateDraw();
                Ui::shouldDrawUpdate = false;
                drawTimer.reset();
            }
        }
    }

    void switchState(State newState) {
        if (currentHandler != nullptr) {
            currentHandler->onExit();
        }

        lastState = currentState;
        currentState = newState;
        currentHandler = getStateHandler(newState);

        if (currentHandler != nullptr) {
            currentHandler->onEnter();
            currentHandler->onInitialDraw();
        }

        if (newState != State::SCREENSAVER) {
            EepromSettings.lastKnownState = newState;
            EepromSettings.markDirty();          
        }
    
    }

    static StateHandler *getStateHandler(State state) {
        #define STATE_FACTORY(s, c) \
            case s: \
                return new (&stateBuffer) c(); \
                break;

        switch (state) {
            STATE_FACTORY(State::HOME, HomeStateHandler);
            STATE_FACTORY(State::HOME_SIMPLE, HomeSimpleStateHandler);
            STATE_FACTORY(State::HOME_STATS, HomeStatsStateHandler);
            STATE_FACTORY(State::SCREENSAVER, ScreensaverStateHandler);
            STATE_FACTORY(State::SEARCH, SearchStateHandler);
            STATE_FACTORY(State::BANDSCAN, BandScanStateHandler);
            STATE_FACTORY(State::MENU, MenuStateHandler);
            STATE_FACTORY(State::SETTINGS, SettingsStateHandler);
            STATE_FACTORY(State::SETTINGS_INTERNAL, SettingsInternalStateHandler);
            STATE_FACTORY(State::SETTINGS_RSSI, SettingsRssiStateHandler);
            STATE_FACTORY(State::SPECTATOR, SpectatorStateHandler);
            STATE_FACTORY(State::FAVOURITES, FavouritesStateHandler);
            STATE_FACTORY(State::FINDER, FinderStateHandler);
            STATE_FACTORY(State::LAPTIMER, LaptimerStateHandler);
            STATE_FACTORY(State::CUSTOMLOGO, CustomLogoStateHandler);

            default:
                return nullptr;
        }

        #undef STATE_FACTORY
    }

    static void onButtonChange(Button button, Buttons::PressType pressType) {
        if (currentHandler != nullptr) {
            currentHandler->onButtonChange(button, pressType);
        }
    }
}
