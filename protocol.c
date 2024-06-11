#include "protocol.h"

uint16_t calculate_checksum(uint8_t *buffer, uint16_t length) {
    uint16_t x = 1;
    uint16_t counter = 0;

    for (uint16_t i = 0; i < length; i++) {
        x = (x + counter + buffer[i]) % 65521;
        counter = (counter + 1) % 256;
    }

    return x;
}
