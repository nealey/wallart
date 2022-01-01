#pragma once

#include <stdint.h>

class Picker {
public:
    Picker();
    bool Pick(uint8_t);
private:
    uint8_t val;
};
