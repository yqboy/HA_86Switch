//
// Created by yq on 2023/9/12.
//

#ifndef HTML_H
#define HTML_H

#include "config.h"
#include "lfs.h"
#include <DNSServer.h>
#include <WebServer.h>
#include <Update.h>
#include <StreamString.h>

DNSServer dnsServer;
WebServer httpServer(80);

static const char HTTP_HTML[] PROGMEM = R"(
<!DOCTYPE html>
<html lang='zh-CN'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/>
    <style>
        h4 {
            margin: 20px 0 0;
        }

        h5 {
            margin: 10px 0 0;
        }

        label {
            font-size: .6rem;
        }

        select {
            padding: 5px;
            font-size: 1em;
            margin-bottom: 5px;
            width: 99%;
        }

        input {
            padding: 5px;
            font-size: 1em;
            margin-bottom: 5px;
            width: 95%
        }

        body {
            text-align: center;
        }

        button {
            border: 0;
            border-radius: 1rem;
            background-color: #1fa3ec;
            color: #fff;
            line-height: 2rem;
            font-size: 1rem;
            width: 100%;
            margin: 1rem 0;
        }

        .file {
            position: relative;
            text-align: center;
            border-radius: 1rem;
            background-color: #1fa3ec;
            color: #fff;
            line-height: 2rem;
        }

        .file input {
            position: absolute;
            right: 0;
            top: 0;
            opacity: 0;
        }

        .container{
            text-align:left;
            display:inline-block;
            min-width:260px;
        }
    </style>
</head>
<body>
<div class='container'>
{container}
</div>
</body>
</html>
)";
const char HTTP_FORM_DATA[] PROGMEM = R"(
    <h4>固件版本: {version}</h4>
    <h5>主机名: {hostname}</h5>
    <form action='save' method='GET'>
        <h4>WiFi 配置</h4>
        <label>SSID:</label>
        <input name='ssid' value='{ssid}'>
        <br/>
        <label>密码:</label>
        <input name='pwd' value='{pwd}'>
        <br/>
        <h4>MQTT 配置</h4>
        <label>Host 地址:</label>
        <input name='host' value='{host}'>
        <br/>
        <label>别名:</label>
        <input name='nodeID' value={nodeID}>
        <br/>
        <h4>自动注册</h4>
        <select name='discovery'>
            <option selected disabled>{discovery}</option>
            <option value='1'>启用</option>
            <option value='0'>禁用</option>
        </select>
        <br/>
        <button type='submit'>保存</button>
    </form>
    <form action='upload' method='POST' enctype='multipart/form-data'>
        <div class="file">
            <input type='file' accept='.bin' name='firmware' onchange='submit()'>升级</input>
        </div>
    </form>
    <form action='reset' method='GET'>
        <button type='submit'>重置</button>
    </form>
)";
const char HTTP_SAVED[] PROGMEM = "<div><br/>正在保存配置并重启...</div>";
const char HTTP_FIRMWARE[] PROGMEM = "<div><br/>升级{firmware}并重启...</div>";
const char HTTP_CLEAN[] PROGMEM = "<div><br/>正在清除配置并重启...</div>";

ulong mill;

void handleRoot()
{
    String page = FPSTR(HTTP_HTML);
    page.replace("{container}", HTTP_FORM_DATA);
    page.replace("{version}", VERSION);
    page.replace("{hostname}", hostname);
    page.replace("{ssid}", mConfig.ssid);
    page.replace("{pwd}", mConfig.pwd);
    page.replace("{host}", mConfig.host);
    page.replace("{nodeID}", mConfig.nodeID);
    page.replace("{discovery}", mConfig.discovery == "1" ? "启用" : "禁用");
    httpServer.sendHeader("Connection", "close");
    httpServer.send(200, "text/html", page);
    mill = millis();
}

void handleSave()
{
    mConfig.ssid = httpServer.arg("ssid");
    mConfig.pwd = httpServer.arg("pwd");
    mConfig.host = httpServer.arg("host");
    mConfig.nodeID = httpServer.arg("nodeID");
    String discovery = httpServer.arg("discovery");
    if (discovery != "")
        mConfig.discovery = discovery;

    FSWrite();
    String page = FPSTR(HTTP_HTML);
    page.replace("{container}", HTTP_SAVED);
    httpServer.sendHeader("Connection", "close");
    httpServer.send(200, "text/html", page);
    delay(100);
    ESP.restart();
}

void handleFirmware()
{
    String page = FPSTR(HTTP_HTML);
    page.replace("{container}", HTTP_FIRMWARE);
    page.replace("{firmware}", (Update.hasError()) ? "失败" : "成功");
    httpServer.sendHeader("Connection", "close");
    httpServer.send(200, "text/html", page);
    delay(100);
    ESP.restart();
}

void handleUpload()
{
    HTTPUpload &upload = httpServer.upload();
    if (upload.status == UPLOAD_FILE_START)
        Update.begin();
    else if (upload.status == UPLOAD_FILE_WRITE)
        Update.write(upload.buf, upload.currentSize);
    else if (upload.status == UPLOAD_FILE_END)
        Update.end(true);
}

void handleReset()
{
    LittleFS.format();
    httpServer.sendHeader("Connection", "close");
    httpServer.send(200);
    String page = FPSTR(HTTP_HTML);
    page.replace("{container}", HTTP_CLEAN);
    httpServer.sendHeader("Connection", "close");
    httpServer.send(200, "text/html", page);
    delay(100);
    ESP.restart();
}

void WebInit()
{
    WiFi.softAP(hostname);

    dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    httpServer.on("/", handleRoot);
    httpServer.on("/save", handleSave);
    httpServer.on("/upload", HTTP_POST, handleFirmware, handleUpload);
    httpServer.on("/reset", handleReset);
    httpServer.onNotFound(handleRoot);
    httpServer.begin();
    mill = millis();
    for (;;)
    {
        if ((millis() - mill) > 1000 * 60 * 1)
            break;

        dnsServer.processNextRequest();
        httpServer.handleClient();
        delay(5);
    }
    dnsServer.stop();
    httpServer.stop();
    WiFi.enableAP(false);
}

#endif // HTML_H
