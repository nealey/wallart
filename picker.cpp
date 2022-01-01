#include <Arduino.h>
#include <stdint.h>
#include "picker.h"

Picker::Picker() {
    val = random(1, 256);
}

bool Picker::Pick(uint8_t likelihood) {
    if ((val > 0) && (val <= likelihood)) {
        val = 0;
        return true;
    }
    val -= likelihood;
    return false;
}