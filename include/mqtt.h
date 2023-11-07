//
// Created by yq on 2023/9/24.
//

#ifndef ESP32_WROOM_86_MQTT_H
#define ESP32_WROOM_86_MQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include "config.h"
#include "gpio.h"

WiFiClient espClient;
PubSubClient mqClient(espClient);

void MQttPubConfig(String k)
{
    delay(100);
    String topic = "homeassistant/switch/" + hostname + "/" + k + "/config";
    String payload = "{\"name\":\"" + k + "\",\"~\": \"homeassistant/switch/" + nodeID + "/" + k + "\",\"cmd_t\":\"~/set\",\"stat_t\":\"~/state\",\"uniq_id\":\"" + hostname + "-" + k + "\",\"device\":{\"ids\":[\"" + hostname + "\"],\"name\":\"" + hostname + "\",\"mf\":\"虤虎科技\",\"mdl\":\"M6智能面板\"}}";

    mqClient.publish(topic.c_str(), payload.c_str(), true);
}

void MQttPub(String t, String payload)
{
    if (mqClient.connected())
    {
        String topic = "homeassistant/switch/" + nodeID + "/" + t;
        mqClient.publish(topic.c_str(), payload.c_str());
    }
}

void pushState()
{
    int val = digitalRead(IO1);
    MQttPub("1/state", val == 0 ? "OFF" : "ON");
#if CONFIG_TWO_SWITCH || CONFIG_THREE_SWITCH
    val = digitalRead(IO2);
    MQttPub("2/state", val == 0 ? "OFF" : "ON");
#endif
#if CONFIG_THREE_SWITCH
    val = digitalRead(IO3);
    MQttPub("3/state", val == 0 ? "OFF" : "ON");
#endif
}

void MQttConnect()
{
    if (mConfig.host == "")
        return;

    if (WiFi.status() != WL_CONNECTED)
        return;

    int port = 1883;
    int i = mConfig.host.indexOf(':');
    if (i != -1)
        port = atoi(mConfig.host.substring(i + 1, mConfig.host.length()).c_str());

    if (!mqClient.connected())
    {
        String host = mConfig.host.substring(0, i);
        mqClient.setServer(host.c_str(), port);
        if (mqClient.connect(hostname.c_str()))
        {
            String sub = "homeassistant/switch/" + nodeID + "/#";
            mqClient.subscribe(sub.c_str());

            if (mConfig.discovery == "1")
            {
                MQttPubConfig("1");
#if CONFIG_TWO_SWITCH || CONFIG_THREE_SWITCH
                MQttPubConfig("2");
#endif
#if CONFIG_THREE_SWITCH
                MQttPubConfig("3");
#endif
                delay(1000);
                pushState();
            }
        }
    }
}

void MQttLoop()
{
    mqClient.loop();
    MQttConnect();
}

#endif // ESP32_WROOM_86_MQTT_H
