#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <arpa/inet.h>

/* Needed for the linker to find it. To convert an IP to in_addr, use:
    struct in_addr addr;
    inet_pton(AF_INET, "192.168.1.1", &addr);
 */
bool is_private_addr(struct in_addr addr, bool filter_test_nets);

#endif /* UTILS_H */
