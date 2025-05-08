#include "commands.h"

FeedBack punisher(PUNISH_PIN, 500);
FeedBack rewarder(REWARD_PIN, 100);

FeedBack::FeedBack(uint8_t pin, unsigned long duration)
    : pin(pin), duration(duration), startTime(0), active(false) {
    pinMode(pin, OUTPUT);            
    digitalWrite(pin, LOW);          
}

void FeedBack::trigger(){
    digitalWrite(pin, HIGH);
    startTime = millis();
    active = true;
}

void FeedBack::update() {
    if (active && millis() - startTime >= duration) {
        digitalWrite(pin, LOW);
        active = false;
    }
}
