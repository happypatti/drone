#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

#define MAX_PAYLOAD_LENGTH 64

typedef struct {
    uint16_t magic;
    uint16_t length;
    uint8_t message_type;
    uint64_t timestamp;
    uint8_t counter;
    uint16_t payload_cs;
    uint16_t header_cs;
    char payload[MAX_PAYLOAD_LENGTH];
} __attribute__((packed)) Message;

uint16_t calculate_checksum(const uint8_t *buffer, size_t length);

#endif // PROTOCOL_H
