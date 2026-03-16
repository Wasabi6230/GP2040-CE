#include "wifi/WifiDriver.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/apps/httpd.h"
#include "lwip/timeouts.h"

#include <stdio.h>

static bool wifi_ready = false;

void wifi_init_ap() {

    if (cyw43_arch_init()) {
        printf("CYW43 init failed\n");
        return;
    }

    cyw43_arch_enable_ap_mode(
        "GP2040-Config",
        "pokemon123",
        CYW43_AUTH_WPA2_AES_PSK
    );

    printf("WiFi AP started\n");

    httpd_init();

    wifi_ready = true;
}

void wifi_task() {

    if (!wifi_ready)
        return;

    cyw43_arch_poll();

    cyw43_arch_poll();
    sys_check_timeouts();
    sleep_ms(2);
}