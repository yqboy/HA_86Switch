//
// Created by yq on 2023/9/16.
//

#ifndef ESP32_WROOM_86_GPIO_H
#define ESP32_WROOM_86_GPIO_H

#include "html.h"
#include "config.h"
#include "mqtt.h"

void GPIOInit()
{
    pinMode(L0, INPUT_PULLUP);
    pinMode(IO1, OUTPUT);
    pinMode(IO2, OUTPUT);
    pinMode(IO3, OUTPUT);
    digitalWrite(IO1, LOW);
    digitalWrite(IO2, LOW);
    digitalWrite(IO3, LOW);

    pinMode(LED1, OUTPUT);
    digitalWrite(LED1, HIGH);
    btn1.attach(BTN1, INPUT_PULLUP);
    btn2.attach(BTN2, INPUT_PULLUP);
    btn1.interval(debounceDelay);
    btn2.interval(debounceDelay);

#if CONFIG_TWO_SWITCH || CONFIG_THREE_SWITCH
    pinMode(LED2, OUTPUT);
    digitalWrite(LED2, HIGH);
    btn3.attach(BTN3, INPUT_PULLUP);
    btn4.attach(BTN4, INPUT_PULLUP);
    btn3.interval(debounceDelay);
    btn4.interval(debounceDelay);
#endif
#if CONFIG_THREE_SWITCH
    pinMode(LED3, OUTPUT);
    digitalWrite(LED3, HIGH);
    btn5.attach(BTN5, INPUT_PULLUP);
    btn6.attach(BTN6, INPUT_PULLUP);
    btn5.interval(debounceDelay);
    btn6.interval(debounceDelay);
#endif
}

void PubBtnState(uint8_t pin, uint8_t v)
{
    String val = v == 0 ? "OFF" : "ON";
    switch (pin)
    {
    case IO1:
        MQttPub("1/state", val);
        break;
    case IO2:
        MQttPub("2/state", val);
        break;
    case IO3:
        MQttPub("3/state", val);
        break;
    case BTN1:
        MQttPub("adv", "1");
        break;
#if CONFIG_TWO_SWITCH || CONFIG_THREE_SWITCH
    case BTN2:
        MQttPub("adv", "2");
        break;
#endif
#if CONFIG_THREE_SWITCH
    case BTN3:
        MQttPub("adv", "3");
        break;
#endif
    }
}

void toggleIO(int pin, int state)
{
    if (pin == IO1 || pin == IO2 || pin == IO3)
    {
        for (;;)
        {
            if (!digitalRead(L0))
                break;
        }
    }
    digitalWrite(pin, state);
}

void toggleIO(int pin)
{
    int val = digitalRead(pin);
    toggleIO(pin, !val);
    PubBtnState(pin, !val);
}

void ConfLoop()
{
    btn1.update();
    if (btn1.read() == LOW && btn1.currentDuration() > 2000)
    {
        ledTicker.attach(0.5, toggleIO, LED1);
        WebInit();
        ledTicker.detach();
        digitalWrite(LED1, !digitalRead(IO1));
    }

    if (btn1.pressed() && btn1.previousDuration() < 500)
        PubBtnState(BTN1, 0);
}

void BtnLoop()
{
    btn2.update();

#if CONFIG_ONE_SWITCH
    if (btn2.released())
    {
        toggleIO(IO1);
        toggleIO(LED1);
    }
#endif
#if CONFIG_TWO_SWITCH
    btn3.update();
    btn4.update();

    if (btn2.pressed())
        PubBtnState(BTN2, 0);

    if (btn3.released())
    {
        toggleIO(IO1);
        toggleIO(LED1);
    }
    if (btn4.released())
    {
        toggleIO(IO2);
        toggleIO(LED2);
    }
#endif
#if CONFIG_THREE_SWITCH
    btn3.update();
    btn4.update();
    btn5.update();
    btn6.update();

    if (btn2.pressed())
        PubBtnState(BTN2, 0);

    if (btn3.pressed())
        PubBtnState(BTN3, 0);

    if (btn4.released())
    {
        toggleIO(IO1);
        toggleIO(LED1);
    }

    if (btn5.released())
    {
        toggleIO(IO2);
        toggleIO(LED2);
    }

    if (btn6.released())
    {
        toggleIO(IO3);
        toggleIO(LED3);
    }
#endif
}

void SwitchToggle(uint8_t pin, uint8_t v, bool state)
{
    if (v == digitalRead(pin))
        return;
    switch (pin)
    {
    case IO1:
        toggleIO(IO1, v);
        toggleIO(LED1, !v);
        break;
#if CONFIG_TWO_SWITCH || CONFIG_THREE_SWITCH
    case IO2:
        toggleIO(IO2, v);
        toggleIO(LED2, !v);
        break;
#endif
#if CONFIG_THREE_SWITCH
    case IO3:
        toggleIO(IO3, v);
        toggleIO(LED3, !v);
        break;
#endif
    }

    if (state)
        PubBtnState(pin, v);
}

#endif // ESP32_WROOM_86_GPIO_H

// #if CONFIG_ONE_SWITCH
// #endif
// #if CONFIG_TWO_SWITCH
// #endif
// #if CONFIG_THREE_SWITCH
// #endif