/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/****************************************************************************
*
* This file is for Eddystone demo. It shows usage of implemented in 
* eddystone_beacon.h Eddystone Beacon types such as Tlm, Uid, Url
*
****************************************************************************/

#include <stdint.h>
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "eddystone_beacon.h"

static const char *DEMO_TAG = "EDDYSTONE-DEMO";

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x800,
    .adv_type = ADV_TYPE_NONCONN_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;

    switch (event)
    {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        {
            ESP_LOGI(DEMO_TAG, "Start advertising");
            esp_ble_gap_start_advertising(&ble_adv_params);
            break;
        }

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            //adv start complete event to indicate adv start successfully or failed
            if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS)
            {
                ESP_LOGE(DEMO_TAG, "Adv start failed: %s", esp_err_to_name(err));
            }
            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS)
            {
                ESP_LOGE(DEMO_TAG, "Adv stop failed: %s", esp_err_to_name(err));
            }
            else
            {
                ESP_LOGI(DEMO_TAG, "Stop adv successfully");
            }
            break;

        default:
            break;
    }
}

void ble_appRegister(void)
{
    esp_err_t status;

    ESP_LOGI(DEMO_TAG, "register callback");
    status = esp_ble_gap_register_callback(esp_gap_cb);

    //register the scan callback function to the gap module
    if (status != ESP_OK)
    {
        ESP_LOGE(DEMO_TAG, "gap register error: %s", esp_err_to_name(status));
    }
}

void ble_beacon_init(void)
{
    esp_bluedroid_init();
    esp_bluedroid_enable();
    ble_appRegister();
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    ble_beacon_init();

    struct eddystone_beacon beacon_tlm, beacon_url, beacon_uid;
    eddystone_beacon_init(&beacon_url, EDDYSTONE_URL);
    eddystone_beacon_init(&beacon_uid, EDDYSTONE_UID);
    eddystone_beacon_init(&beacon_tlm, EDDYSTONE_TLM);
    
    // fill url beacon
    eddystone_url_write(&beacon_url, "google.com");

    // fill uid beacon
    beacon_uid.uid.tx_power = 0x20;
    beacon_uid.uid.NID[0] = 0xDE;
    beacon_uid.uid.NID[1] = 0xAD;

    // fill tlm beacon
    beacon_tlm.tlm.voltage_lsb = LSB_BYTE(3300);
    beacon_tlm.tlm.voltage_msb = MSB_BYTE(3300);
    beacon_tlm.tlm.temperature_lsb = LSB_BYTE(TLM_TEMPERATURE_NOT_SUPPORTED);
    beacon_tlm.tlm.temperature_msb = MSB_BYTE(TLM_TEMPERATURE_NOT_SUPPORTED);
    beacon_tlm.tlm.adv_cnt[0] = 0;
    beacon_tlm.tlm.adv_cnt[1] = 0;
    beacon_tlm.tlm.adv_cnt[2] = 0;
    beacon_tlm.tlm.adv_cnt[3] = 0;
    beacon_tlm.tlm.sec_cnt[0] = 0;
    beacon_tlm.tlm.sec_cnt[1] = 0;
    beacon_tlm.tlm.sec_cnt[2] = 0;
    beacon_tlm.tlm.sec_cnt[3] = 0;    

    // Tell gap controller to advertise beacon_tlm
    esp_ble_gap_config_adv_data_raw(beacon_uid.data, beacon_uid.length);
    
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
