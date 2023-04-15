#include <string.h>

#include "py/builtin.h"
#include "py/runtime.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/obj.h"
#include "py/stream.h"
#include "py/mperrno.h"

#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_log.h"
#include "esp_event.h"

#include "modnetwork.h"


//NORETURN void esp_exceptions_helper(esp_err_t e) {
//    switch (e) {
//        case ESP_ERR_WIFI_NOT_INIT:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Initialized"));
//        case ESP_ERR_WIFI_NOT_STARTED:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Started"));
//        case ESP_ERR_WIFI_NOT_STOPPED:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Stopped"));
//        case ESP_ERR_WIFI_IF:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Interface"));
//        case ESP_ERR_WIFI_MODE:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Mode"));
//        case ESP_ERR_WIFI_STATE:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal State Error"));
//        case ESP_ERR_WIFI_CONN:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal Error"));
//        case ESP_ERR_WIFI_NVS:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal NVS Error"));
//        case ESP_ERR_WIFI_MAC:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid MAC Address"));
//        case ESP_ERR_WIFI_SSID:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi SSID Invalid"));
//        case ESP_ERR_WIFI_PASSWORD:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Password"));
//        case ESP_ERR_WIFI_TIMEOUT:
//            mp_raise_OSError(MP_ETIMEDOUT);
//        case ESP_ERR_WIFI_WAKE_FAIL:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Wakeup Failure"));
//        case ESP_ERR_WIFI_WOULD_BLOCK:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Would Block"));
//        case ESP_ERR_WIFI_NOT_CONNECT:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Connected"));
//        case ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP Invalid Parameters"));
//        case ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP IF Not Ready"));
//        case ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED:
//            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP DHCP Client Start Failed"));
//        case ESP_ERR_TCPIP_ADAPTER_NO_MEM:
//            mp_raise_OSError(MP_ENOMEM);
//        default:
//            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Wifi Unknown Error 0x%04x"), e);
//    }
//}
//
//static inline void esp_exceptions(esp_err_t e) {
//    if (e != ESP_OK) {
//        esp_exceptions_helper(e);
//    }
//}

// Set up EAP
STATIC mp_obj_t esp_seteap(mp_obj_t username,mp_obj_t password){

    // The length of username and password
    size_t Ilen;
    size_t Plen;

    // Converts from micropython form to C form
    const char *EAP_IDENTITY = mp_obj_str_get_data(username,&Ilen);
    const char *EAP_PASSWORD = mp_obj_str_get_data(password,&Plen);

    // Sets the identity and password
    esp_exceptions(esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)));
    esp_exceptions(esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)));
    // esp_exceptions(esp_wifi_sta_wpa2_ent_set_new_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)));
    esp_exceptions(esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)));

    // Enables wpa enterprise
    esp_wifi_sta_wpa2_ent_enable();

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(esp_seteap_obj, esp_seteap);

STATIC const mp_rom_map_elem_t mp_module_enterprise_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_enterprise) },
    { MP_ROM_QSTR(MP_QSTR_seteap), MP_ROM_PTR(&esp_seteap_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_enterprise_globals, mp_module_enterprise_globals_table);

const mp_obj_module_t mp_module_enterprise = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&mp_module_enterprise_globals,
};

MP_REGISTER_MODULE(MP_QSTR_enterprise, mp_module_enterprise, 1);
