/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef _EDDYSTONE_BEACON_H
#define _EDDYSTONE_BEACON_H

#include <stdint.h>

#define EDDYSTONE_TYPE_TLM (0x20)
#define EDDYSTONE_TYPE_URL (0x10)
#define EDDYSTONE_TYPE_UID (0x00)

#define BEACON_ADV_LENGTH (31)
#define EDDYSTONE_UUID (0xFEAA)

/****************/
/* TLM SPECIFIC */
/****************/
#define TLM_VERSION (0x00)
#define TLM_TEMPERATURE_NOT_SUPPORTED (0x8000)
#define TLM_SERVICE_DATA_LENGTH (0x11)
#define TLM_LENGTH (25)

/****************/
/* URL SPECIFIC */
/****************/
#define URL_URL_MAX_LENGTH (17)
#define URL_SERVICE_DATA_BASE_LENGTH (6)

/****************/
/* UID SPECIFIS */
/****************/
#define UID_NAMESPACE_ID_LENGTH (10)
#define UID_INSTANCE_ID_LENGTH (6) 
#define UID_RFU_LENGTH (2)
#define UID_SERVICE_DATA_LENGTH (0x17)

#define MSB_BYTE(data) ((data>>0x08)&0xFF)
#define LSB_BYTE(data) (data&0xFF)

enum eddystone_type {
    EDDYSTONE_UID,
    EDDYSTONE_URL,
    EDDYSTONE_TLM,
};


struct eddystone_header {
    uint8_t section0_len;
    uint8_t type_flags;
    uint8_t modes;
    uint8_t section1_len;
    uint8_t uuid_type;
    uint8_t uuid_lsb;
    uint8_t uuid_msb;

    uint8_t service_data_len; // tlm - always 0x11, url - 6 + url length, uid - always 0x17
    uint8_t type_service_data;
    uint8_t eddy_uuid_lsb;
    uint8_t eddy_uuid_msb;
} __attribute__((packed));


struct eddystone_tlm {
    struct eddystone_header header;
    uint8_t frame_type;
    uint8_t version;
    uint8_t voltage_msb;
    uint8_t voltage_lsb;
    uint8_t temperature_msb;
    uint8_t temperature_lsb;
    uint8_t adv_cnt[4];
    uint8_t sec_cnt[4];
} __attribute__((packed));


struct eddystone_url {
    struct eddystone_header header;
    uint8_t frame_type;
    uint8_t tx_power;
    uint8_t url_scheme;
    uint8_t url[17];
} __attribute__((packed));


struct eddystone_uid {
    struct eddystone_header header;
    uint8_t frame_type; 
    uint8_t tx_power;
    uint8_t NID[UID_NAMESPACE_ID_LENGTH];
    uint8_t BID[UID_INSTANCE_ID_LENGTH];
    uint8_t RFU[UID_RFU_LENGTH]; // 0x00
} __attribute__((packed));


struct eddystone_beacon {
    union {
        uint8_t data[BEACON_ADV_LENGTH];
        struct eddystone_tlm tlm;
        struct eddystone_url url;
        struct eddystone_uid uid;
    };
    uint8_t length;
} __attribute__((packed));


/** @brief Initializes beacon_data as provided eddystone_type
 *  @param beacon_data instance of eddystone_beacon_data struct 
 *  @param type type of beacon required
 */
void eddystone_beacon_init(struct eddystone_beacon *beacon_data, enum eddystone_type type);

/** @brief Writes url to beacon. Beacon must be initialized to EDDYSTONE_URL before.
 * @param beacon beacon instance
 * @param url url that will be written to beacon, no longer than 17 characters
 */
void eddystone_url_write(struct eddystone_beacon *beacon, char *url);


/****************************************************** 
 *
 * These examples show manually filled data structures
 * 
 * struct eddystone_tlm eddystone_tlm_data = {
 *     .header = {
 *         .section0_len = 2,
 *         .type_flags = 0x01,
 *         .modes = 0x06,
 *         .section1_len = 3,
 *         .uuid_type = 0x03,
 *         .uuid_lsb = LSB_BYTE(EDDYSTONE_UUID),
 *         .uuid_msb = MSB_BYTE(EDDYSTONE_UUID),
 * 
 *         .service_data_len = 0x11,
 *         .type_service_data = 0x16,
 *         .eddy_uuid_lsb = LSB_BYTE(EDDYSTONE_UUID),
 *         .eddy_uuid_msb = MSB_BYTE(EDDYSTONE_UUID),
 *     },
 * 
 *     .frame_type = EDDYSTONE_TYPE_TLM,
 *     .version = 0,
 *     .voltage_msb = (3300>>8),
 *     .voltage_lsb = (3300&0xFF),
 *     .temperature_msb = MSB_BYTE(TLM_TEMPERATURE_NOT_SUPPORTED),
 *     .temperature_lsb = LSB_BYTE(TLM_TEMPERATURE_NOT_SUPPORTED),
 *     .adv_cnt = {0},
 *     .sec_cnt = {0},
 * };
 * 
 * struct eddystone_url eddystone_url_data = {
 *     .header = {
 *         .section0_len = 2,
 *         .type_flags = 0x01,
 *         .modes = 0x06,
 *         .section1_len = 3,
 *         .uuid_type = 0x03,
 *         .uuid_lsb = LSB_BYTE(EDDYSTONE_UUID),
 *         .uuid_msb = MSB_BYTE(EDDYSTONE_UUID),
 * 
 *         .service_data_len = 6 + strlen("google.com"),
 *         .type_service_data = 0x16,
 *         .eddy_uuid_lsb = LSB_BYTE(EDDYSTONE_UUID),
 *         .eddy_uuid_msb = MSB_BYTE(EDDYSTONE_UUID),
 *     },
 * 
 *     .frame_type = EDDYSTONE_TYPE_URL,
 *     .tx_power = 0x20,
 *     .url_scheme = 0x00,
 *     .url = "google.com"
 * };
 * 
 * struct eddystone_uid eddystone_uid_data = {
 *     .header = {
 *         .section0_len = 2,
 *         .type_flags = 0x01,
 *         .modes = 0x06,
 *         .section1_len = 3,
 *         .uuid_type = 0x03,
 *         .uuid_lsb = LSB_BYTE(EDDYSTONE_UUID),
 *         .uuid_msb = MSB_BYTE(EDDYSTONE_UUID),
 * 
 *         .service_data_len = UID_SERVICE_DATA_LENGTH,
 *         .type_service_data = 0x16,
 *         .eddy_uuid_lsb = LSB_BYTE(EDDYSTONE_UUID),
 *         .eddy_uuid_msb = MSB_BYTE(EDDYSTONE_UUID),
 *     },
 *     .tx_power = 0x20,
 *     .frame_type = EDDYSTONE_TYPE_UID,
 *     .NID = {0},
 *     .BID = {0},
 *     .RFU = {0}
 * };
 * 
******************************************************/

#endif /* _EDDYSTONE_BEACON_H */
