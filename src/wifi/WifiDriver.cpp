#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/apps/httpd.h"

void wifi_init_ap() {

    if (cyw43_arch_init()) {
        printf("WiFi init failed\n");
        return;
    }

    cyw43_arch_enable_ap_mode(
        "GP2040-Config",
        "pokemon123",
        CYW43_AUTH_WPA2_AES_PSK
    );

    printf("WiFi AP started\n");

    start_gp2040_web_server();
}