#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

uint16_t calculate_checksum(uint8_t *buffer, uint16_t length);

#endif
