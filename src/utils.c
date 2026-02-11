#include "bose-hook/utils.h"

bool is_private_addr(struct in_addr addr, bool filter_test_nets)
{
    uint32_t ip = ntohl(addr.s_addr);
    // Note: we consider 0.0.0.0/8 as public for our purposes because it is often used by Adblockers.

    // --- ALWAYS PRIVATE (RFC 1918) ---
    if ((ip >= 0x0A000000 && ip <= 0x0AFFFFFF) || // 10.0.0.0/8
        (ip >= 0xAC100000 && ip <= 0xAC1FFFFF) || // 172.16.0.0/12
        (ip >= 0xC0A80000 && ip <= 0xC0A8FFFF))   // 192.168.0.0/16
        return true;

    // --- ALWAYS PRIVATE (Link-Local & Loopback) ---
    if ((ip & 0xFFFF0000) == 0xA9FE0000)          // 169.254.0.0/16
        return true;
    if ((ip & 0xFF000000) == 0x7F000000)          // 127.0.0.0/8
        return true;

    // --- OPTIONAL: TEST-NETS & CGNAT ---
    if (filter_test_nets) {
        // CGNAT (100.64.0.0/10)
        if ((ip >= 0x64400000 && ip <= 0x647FFFFF))
            return true;

        // TEST-NET-1, 2, 3
        if ((ip & 0xFFFFFF00) == 0xC0000200) return true; // 192.0.2.0/24
        if ((ip & 0xFFFFFF00) == 0xC6336400) return true; // 198.51.100.0/24
        if ((ip & 0xFFFFFF00) == 0xCB007100) return true; // 203.0.113.0/24
    }

    return false;
}
