//
// Created by langelog on 1/22/20.
//

#ifndef LORAMODULE_BUILTINLED_H
#define LORAMODULE_BUILTINLED_H

#include <Arduino.h>

// since there is one builtin led, this class should be singleton
class BuiltInLed {

protected:
    BuiltInLed();

    static BuiltInLed* builtInLed_;

public:

    BuiltInLed(BuiltInLed& other) = delete; // should not be cloneable
    void operator=(const BuiltInLed&) = delete; // should not be assignable

    static BuiltInLed* GetInstance();

    void blinkLED();

private:
    bool ledStatus;
};

#endif //LORAMODULE_BUILTINLED_H
