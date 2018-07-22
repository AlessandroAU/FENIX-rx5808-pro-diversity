#ifndef STATE_HOME_H
#define STATE_HOME_H


#include "state.h"
#include "settings.h"


namespace StateMachine {
    class HomeStateHandler : public StateMachine::StateHandler {
        private:
            void setChannel(int channelIncrement);

        public:
            void onEnter();
            void onUpdate();

            void onInitialDraw();
            void onUpdateDraw();

            void onButtonChange(Button button, Buttons::PressType pressType);

    };
}

#endif
