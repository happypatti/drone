#include "protocol.h"
#include <stdio.h>


uint16_t calculate_checksum(const uint8_t *buffer, size_t length) {
    uint32_t x = 1;
    uint8_t counter = 0;
    for (size_t i = 0; i < length; i++) {
        x = (x + counter + buffer[i]) % 65521;
        counter = (counter + 1) % 256;
        printf("Checksum calc byte %d: val=%02x, sum=%u\n", i, buffer[i], x);
    }
    return (uint16_t)x;
}

