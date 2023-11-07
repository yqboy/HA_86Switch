//
// Created by yq on 2023/9/17.
//

#ifndef ESP32_WROOM_86_LFS_H
#define ESP32_WROOM_86_LFS_H

#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>

typedef struct
{
    String ssid;
    String pwd;
    String host;
    String nodeID;
    String discovery;
} config;
config mConfig;

void FSWrite()
{
    LittleFS.begin(true);
    File fs = LittleFS.open("/cfg", FILE_WRITE);
    fs.println(mConfig.ssid);
    fs.println(mConfig.pwd);
    fs.println(mConfig.host);
    fs.println(mConfig.nodeID);
    fs.println(mConfig.discovery);
    fs.close();
    LittleFS.end();
}

void FSRead()
{
    if (!LittleFS.begin(true))
        return;
    if (!LittleFS.exists("/cfg"))
        return;

    size_t i = 0;
    File fs = LittleFS.open("/cfg", FILE_READ);

    while (fs.available())
    {
        i++;
        String data = fs.readStringUntil('\n');
        data.trim();
        switch (i)
        {
        case 1:
            mConfig.ssid = data;
            break;
        case 2:
            mConfig.pwd = data;
            break;
        case 3:
            mConfig.host = data;
            break;
        case 4:
            mConfig.nodeID = data;
            break;
        case 5:
            mConfig.discovery = data;
            break;
        }
    }
    fs.close();
    LittleFS.end();
}

#endif // ESP32_WROOM_86_LFS_H
