//
// Created by langelog on 1/22/20.
//

#include <BuiltInLed.h>

BuiltInLed* BuiltInLed::builtInLed_ = nullptr;

BuiltInLed* BuiltInLed::GetInstance() {
    if(builtInLed_ == nullptr) {
        builtInLed_ = new BuiltInLed();
    }
    return builtInLed_;
}

BuiltInLed::BuiltInLed() {
    ledStatus = HIGH;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledStatus);
}

void BuiltInLed::blinkLED() {
    digitalWrite(LED_BUILTIN, ledStatus);
    ledStatus = !ledStatus;
}
