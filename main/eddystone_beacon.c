#include "eddystone_beacon.h"
#include <string.h>

const struct eddystone_header default_header = {
    .section0_len = 2,
    .type_flags = 0x01,
    .modes = 0x06,
    .section1_len = 3,
    .uuid_type = 0x03,
    .uuid_lsb = LSB_BYTE(EDDYSTONE_UUID),
    .uuid_msb = MSB_BYTE(EDDYSTONE_UUID),
    .service_data_len = 0,
    .type_service_data = 0x16,
    .eddy_uuid_lsb = LSB_BYTE(EDDYSTONE_UUID),
    .eddy_uuid_msb = MSB_BYTE(EDDYSTONE_UUID)
}; 

void eddystone_beacon_init(struct eddystone_beacon *beacon, enum eddystone_type beacon_type) {

    struct eddystone_header eddystone_header = default_header;

    switch(beacon_type) {
        case EDDYSTONE_UID: {
            eddystone_header.service_data_len = UID_SERVICE_DATA_LENGTH;
            struct eddystone_uid eddystone_uid = {
                .header = eddystone_header,
                .frame_type = EDDYSTONE_TYPE_UID,
                .RFU = {0},
                .NID = {0},
                .BID = {0}
            };
            memcpy(beacon->data, &eddystone_uid, sizeof(eddystone_uid));
            beacon->length = BEACON_ADV_LENGTH;
            break;
        }

        case EDDYSTONE_URL: {
            eddystone_header.service_data_len = URL_SERVICE_DATA_BASE_LENGTH;
            struct eddystone_url eddystone_url = {
                .header = eddystone_header,
                .frame_type = EDDYSTONE_TYPE_URL,
            };
            memcpy(beacon->data, &eddystone_url, sizeof(eddystone_url));
            beacon->length = (BEACON_ADV_LENGTH - URL_URL_MAX_LENGTH);
            break;
        }

        case EDDYSTONE_TLM: {
            eddystone_header.service_data_len = TLM_SERVICE_DATA_LENGTH;
            struct eddystone_tlm eddystone_tlm = {
                .header = eddystone_header,
                .frame_type = EDDYSTONE_TYPE_TLM,
                .version = TLM_VERSION,
                .adv_cnt = {0},
                .sec_cnt = {0}
            };  
            memcpy(beacon->data, &eddystone_tlm, sizeof(eddystone_tlm));
            beacon->length = TLM_LENGTH;
            break;
        }
    }    
}

void eddystone_url_write(struct eddystone_beacon *beacon, char *url) {

    uint8_t url_len = strlen(url);
    struct eddystone_url *eddystone_url = (struct eddystone_url*)beacon->data;
    
    if(url_len > URL_URL_MAX_LENGTH) {
        url_len = URL_URL_MAX_LENGTH;
    }

    eddystone_url->header.service_data_len = URL_SERVICE_DATA_BASE_LENGTH + url_len;
    eddystone_url->tx_power = 0x20;
    eddystone_url->url_scheme = 0x03;
    memcpy(eddystone_url->url, url, url_len);
    beacon->length = BEACON_ADV_LENGTH - (URL_URL_MAX_LENGTH-url_len);
}
