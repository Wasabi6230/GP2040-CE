#include "WifiDriver.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include <cstdio>

extern "C" {
#include "lwip/apps/httpd.h"
#include "lwip/timeouts.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/err.h"
#include "lwip/def.h"

#include "dhserver.h"
#include "dnserver.h"
}

#define INIT_IP4(a, b, c, d) { PP_HTONL(LWIP_MAKEU32(a, b, c, d)) }

static bool wifi_ready = false;

// AP IP settings
static const ip4_addr_t ap_ip      = INIT_IP4(192, 168, 4, 1);
static const ip4_addr_t ap_netmask = INIT_IP4(255, 255, 255, 0);
static const ip4_addr_t ap_router  = INIT_IP4(192, 168, 4, 1);

// DHCP lease table
static dhcp_entry_t entries[] = {
    {{0}, INIT_IP4(192, 168, 4, 2), 24 * 60 * 60},
    {{0}, INIT_IP4(192, 168, 4, 3), 24 * 60 * 60},
    {{0}, INIT_IP4(192, 168, 4, 4), 24 * 60 * 60},
    {{0}, INIT_IP4(192, 168, 4, 5), 24 * 60 * 60},
};

static const dhcp_config_t dhcp_config = {
    .router = INIT_IP4(192, 168, 4, 1),
    .port   = 67,
    .dns    = INIT_IP4(192, 168, 4, 1),
    "gp2040",
    sizeof(entries) / sizeof(entries[0]),
    entries
};

static bool dns_query_proc(const char *name, ip4_addr_t *addr) {
    LWIP_UNUSED_ARG(name);
    *addr = ap_ip;
    return true;
}

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

    netif_set_addr(netif_default, &ap_ip, &ap_netmask, &ap_router);

    while (dhserv_init(&dhcp_config) != ERR_OK) {
        sleep_ms(10);
    }

    while (dnserv_init(IP_ADDR_ANY, 53, dns_query_proc) != ERR_OK) {
        sleep_ms(10);
    }

    httpd_init();

    printf("WiFi AP started at 192.168.4.1\n");
    wifi_ready = true;
}

void wifi_task() {
    if (!wifi_ready) return;
    cyw43_arch_poll();
    sys_check_timeouts();
    sleep_ms(2);
}