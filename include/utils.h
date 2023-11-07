//
// Created by yq on 2023/9/24.
//

#ifndef ESP32_WROOM_86_UTILS_H
#define ESP32_WROOM_86_UTILS_H

#include <Arduino.h>

void strToUint(String str, uint8_t *data)
{
    size_t k = 0;
    for (size_t i = 0; i < str.length(); i += 2)
    {
        data[k] = strtoul(str.substring(i, i + 2).c_str(), NULL, 16);
        k++;
    }
}

#endif // ESP32_WROOM_86_UTILS_H
