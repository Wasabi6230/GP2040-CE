#include "WifiDriver.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/httpd.h"
#include "lwip/timeouts.h"
#include "lwip/ip4_addr.h"

// These headers come from the TinyUSB networking support you already build
#include "dhcpserver.h"
#include "dnsserver.h"

static bool wifi_ready = false;
static ip_addr_t ap_gw;
static ip4_addr_t ap_mask;
static dhcp_server_t dhcp_server;
static dns_server_t dns_server;

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

#if LWIP_IPV6
    #define IP4(x) ((x).u_addr.ip4)
#else
    #define IP4(x) (x)
#endif

    IP4(ap_gw).addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    IP4(ap_mask).addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

#undef IP4

    dhcp_server_init(&dhcp_server, &ap_gw, &ap_mask);
    dns_server_init(&dns_server, &ap_gw);

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