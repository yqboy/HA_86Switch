//
// Created by yq on 2023/9/17.
//

#ifndef ESP32_WROOM_86_BLE_H
#define ESP32_WROOM_86_BLE_H

#include <Arduino.h>
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "utils.h"

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_NONCONN_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

void BleInit()
{
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    delay(200);
}

void BleAdv(String str)
{
    size_t l = str.length() / 2;
    uint8_t data[l];
    strToUint(str, data);
    esp_ble_gap_config_adv_data_raw(data, sizeof(data));
    esp_ble_gap_start_advertising(&ble_adv_params);
    delay(200);
    esp_ble_gap_stop_advertising();
}

#endif // ESP32_WROOM_86_BLE_H
