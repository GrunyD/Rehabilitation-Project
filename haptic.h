#ifndef HAPTIC_H
#define HAPTIC_H

#include <Arduino.h>

#define D0 2
#define D1 3
#define D2 4
#define D3 5
#define D4 6
#define D5 7
#define D6 21
#define D7 20
#define D8 8
#define D9 9
#define D10 10

#define PUNISH_PIN 5
#define REWARD_PIN 4

#define PUNISH_COMMAND 'P'
#define REWARD_COMMAND 'R'
#define CALIBRATE_COMMAND 'C'

namespace haptic{

    class FeedBack {
    public:
        FeedBack(uint8_t pin, unsigned long duration);
        void trigger();     // Call this when command is received
        void update();      // Call this in your loop()

    private:
        uint8_t pin;
        unsigned long duration;
        unsigned long startTime;
        bool active;
    };
    
    extern FeedBack punisher;
    extern FeedBack rewarder;
}


#endif