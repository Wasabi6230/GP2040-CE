#pragma once
#include <stdint.h>

// Initialize Wi-Fi access point and HTTP server
void wifi_init_ap();

// Call this in a loop on core 1 to poll the Wi-Fi stack
void wifi_poll_loop();