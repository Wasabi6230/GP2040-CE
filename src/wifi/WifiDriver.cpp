#include "WifiDriver.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/apps/httpd.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"

#include "fs.h"

#include <stdio.h>

static bool wifi_ready = false;

static void log_network_interfaces() {
    const struct netif* netif = netif_list;

    if (netif == nullptr) {
        printf("[wifi] no lwIP netif registered yet; expected AP IP is 192.168.4.1\n");
        return;
    }

    while (netif != nullptr) {
        printf(
            "[wifi] netif %c%c%u ip=%s gw=%s mask=%s\n",
            netif->name[0],
            netif->name[1],
            netif->num,
            ip4addr_ntoa(netif_ip4_addr(netif)),
            ip4addr_ntoa(netif_ip4_gw(netif)),
            ip4addr_ntoa(netif_ip4_netmask(netif))
        );
        netif = netif->next;
    }
}

void wifi_init_ap() {
    printf("[wifi] wifi_init_ap reached\n");

    if (cyw43_arch_init()) {
        printf("[wifi] CYW43 init failed\n");
        return;
    }

    cyw43_arch_enable_ap_mode(
        "GP2040-Config",
        "pokemon123",
        CYW43_AUTH_WPA2_AES_PSK
    );

    printf("[wifi] AP started: ssid=GP2040-Config auth=WPA2-PSK\n");
    log_network_interfaces();

    httpd_init();
    printf(
        "[wifi] HTTP server initialized: fs_root=%s files=%d has_root_alias=%s has_index=%s\n",
        fs_debug_root_name() ? fs_debug_root_name() : "(null)",
        fs_debug_numfiles(),
        fs_debug_has_file("/") ? "yes" : "no",
        fs_debug_has_file("/index.html") ? "yes" : "no"
    );

    wifi_ready = true;
}

void wifi_task() {

    if (!wifi_ready)
        return;
    cyw43_arch_poll();
    sys_check_timeouts();
    sleep_ms(2);
}
