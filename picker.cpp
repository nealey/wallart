#include <Arduino.h>
#include <stdint.h>
#include "picker.h"

Picker::Picker() {
    this->val = random(1, 256);
}

bool Picker::Pick(uint8_t likelihood) {
    bool picked = false;
    if ((val > 0) && (val <= likelihood)) {
        picked = true;
    }
    val -= likelihood;
    return picked;
}