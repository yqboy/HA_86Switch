//
// Created by yq on 2023/9/16.
//

#ifndef ESP32_WROOM_86_CONFIG_H
#define ESP32_WROOM_86_CONFIG_H

#include <Arduino.h>
#include <Ticker.h>
#include "Bounce2.h"
#include "lfs.h"

#if CONFIG_ONE_SWITCH
#define VERSION "M6.1.0"
#endif
#if CONFIG_TWO_SWITCH
#define VERSION "M6.2.0"
#endif
#if CONFIG_THREE_SWITCH
#define VERSION "M6.3.0"
#endif

#define L0 26

Bounce2::Button btn1 = Bounce2::Button();
Bounce2::Button btn2 = Bounce2::Button();

#if CONFIG_TWO_SWITCH || CONFIG_THREE_SWITCH
Bounce2::Button btn3 = Bounce2::Button();
Bounce2::Button btn4 = Bounce2::Button();
#endif
#if CONFIG_THREE_SWITCH
Bounce2::Button btn5 = Bounce2::Button();
Bounce2::Button btn6 = Bounce2::Button();
#endif

#if CONFIG_ONE_SWITCH
#define LED1 17

#define BTN1 14
#define BTN2 5
#endif

#if CONFIG_TWO_SWITCH
#define LED1 19
#define LED2 4

#define BTN1 12
#define BTN2 27
#define BTN3 18
#define BTN4 16
#endif

#if CONFIG_THREE_SWITCH
#define LED1 19
#define LED2 17
#define LED3 4

#define BTN1 12
#define BTN2 14
#define BTN3 27
#define BTN4 18
#define BTN5 5
#define BTN6 16

#endif

#define IO1 32
#define IO2 33
#define IO3 25

size_t debounceDelay = 50;

Ticker ledTicker;

String hostname = "AP-";
String nodeID;

#endif // ESP32_WROOM_86_CONFIG_H
