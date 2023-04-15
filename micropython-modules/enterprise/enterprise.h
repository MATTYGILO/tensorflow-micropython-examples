#include "esp_event.h"

NORETURN void esp_exceptions_helper(esp_err_t e);

static inline void esp_exceptions(esp_err_t e) {
    if (e != ESP_OK) {
        esp_exceptions_helper(e);
    }
}