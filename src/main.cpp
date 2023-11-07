#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "gpio.h"
#include "lfs.h"
#include "mqtt.h"
#include "soc/rtc_wdt.h"
#if CONFIG_BLE
#include "ble.h"
#endif

void loop0(void *pv);
void WifiInit();
void MQttCallback(char *topic, uint8_t *payload, uint length);

void setup()
{
    // Serial.begin(115200);
    char buf[2];
    for (int i = 24; i < 41; i = i + 8)
    {
        sprintf(buf, "%02X", (ESP.getEfuseMac() >> i) & 0xff);
        hostname += String(buf);
    }

    FSRead();
    nodeID = mConfig.nodeID == "" ? hostname : mConfig.nodeID;
    GPIOInit();
    WifiInit();

#if CONFIG_BLE
    BleInit();
#endif

    mqClient.setCallback(MQttCallback);
    rtc_wdt_protect_off();
    rtc_wdt_enable();
    rtc_wdt_set_time(RTC_WDT_STAGE0, 20000);
    xTaskCreatePinnedToCore(loop0, "Loop0", 4096, NULL, 0, NULL, 0);
}

void loop()
{
    MQttLoop();
    BtnLoop();
    rtc_wdt_feed();
    delay(5);
}

void loop0(void *pv)
{
    for (;;)
    {
        ConfLoop();
        delay(5);
    }
}

void WifiInit()
{
    WiFi.disconnect(true, true);
    delay(100);
    WiFi.hostname(hostname);
    WiFi.mode(WIFI_AP_STA);
    WiFi.enableAP(false);
    if (mConfig.ssid == "")
        return;
    WiFi.begin(mConfig.ssid, mConfig.pwd);
    // WiFi.waitForConnectResult();
}

void MQttCallback(char *topic, byte *payload, uint length)
{
    String mtopic = String(topic);
    String fCode;

    int i = mtopic.lastIndexOf("/");
    fCode = mtopic.substring(i + 1, mtopic.length());

    String data;
    for (int i = 0; i < length; i++)
        data += (char)payload[i];

    if (fCode == "set" || fCode == "state")
    {
        int j = mtopic.lastIndexOf("/", i - 1);
        String key = mtopic.substring(j + 1, i);
        int val = data == "ON" ? 1 : 0;
        bool state = fCode == "set" ? true : false;

        if (key == "1")
            SwitchToggle(IO1, val, state);
        else if (key == "2")
            SwitchToggle(IO2, val, state);
        else if (key == "3")
            SwitchToggle(IO3, val, state);
    }
#if CONFIG_BLE
    else if (fCode == "ble")
        BleAdv(data);
#endif
    else if (fCode == "reboot")
        ESP.restart();
}