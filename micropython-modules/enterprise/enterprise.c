#include "py/builtin.h"
#include "py/runtime.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/obj.h"
#include "py/stream.h"

#include "esp_wifi.h"
#include "esp_wpa2.h"


//Set up EAP
STATIC mp_obj_t esp_seteap(mp_obj_t username,mp_obj_t password){

    // The length of username and password
    size_t Ilen;
    size_t Plen;

    // Converts from micropython form to C form
    const char *EAP_IDENTITY = mp_obj_str_get_data(username,&Ilen);
    const char *EAP_PASSWORD = mp_obj_str_get_data(password,&Plen);

    // Sets the identity and password
    ESP_EXCEPTIONS(esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)));
    ESP_EXCEPTIONS(esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)));
    //ESP_EXCEPTIONS(esp_wifi_sta_wpa2_ent_set_new_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)));
    ESP_EXCEPTIONS(esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)));

    // Enables wpa enterprise
    ESP_EXCEPTIONS(esp_wifi_sta_wpa2_ent_enable());

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
