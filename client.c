#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h> // Include for offsetof
#include "protocol.h"

// Use the resolved IP address directly
#define SERVER_ADDRESS "3.91.129.142"
#define SERVER_PORT 8162

uint64_t to_little_endian(uint64_t value) {
    return ((uint64_t)htonl(value & 0xFFFFFFFF) << 32) | htonl(value >> 32);
}

void print_message_details(const Message *msg) {
    printf("Message details:\n");
    printf("Magic: 0x%x\n", msg->magic);
    printf("Length: %d\n", msg->length);
    printf("Message Type: %d\n", msg->message_type);
    printf("Timestamp: %llu\n", (unsigned long long)msg->timestamp);
    printf("Counter: %d\n", msg->counter);
    printf("Payload Checksum: 0x%x\n", msg->payload_cs);
    printf("Header Checksum: 0x%x\n", msg->header_cs);
    printf("Payload: %s\n", msg->payload);
}

void print_bytes(const uint8_t *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        printf("%02x ", buffer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void print_payload(const char *payload) {
    printf("Payload: %s\n", payload);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    Message msg;
    char *payload = "[Ali Shalash]"; // Replace with your name

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }
    printf("Socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Convert address using the IP directly
    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }
    printf("Address converted successfully.\n");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    printf("Connected to server successfully.\n");

    // Fill in message details
    msg.magic = 0xf00d; // Little endian
    msg.length = strlen(payload); // Little endian
    msg.message_type = 1; // Request type
    msg.timestamp = ((uint64_t)time(NULL)); // Little-endian for 64-bit value
    msg.counter = 0;
    memset(msg.payload, 0, MAX_PAYLOAD_LENGTH); // Ensure payload is zeroed out
    strncpy(msg.payload, payload, MAX_PAYLOAD_LENGTH - 1); // Ensure null-terminated

    // Calculate payload checksum
    msg.payload_cs = calculate_checksum((uint8_t *)msg.payload, strlen(payload));
    // Calculate header checksum up to this point
    msg.header_cs = calculate_checksum((uint8_t *)&msg, offsetof(Message, payload_cs));

    printf("Message prepared successfully.\n");
    print_message_details(&msg);

    // Print message bytes for debugging
    printf("Message bytes:\n");
    print_bytes((uint8_t *)&msg, sizeof(Message));

    // Send message
    if (send(sock, &msg, sizeof(Message), 0) < 0) {
        perror("Send failed");
        return -1;
    }
    printf("Message sent successfully.\n");

    // Receive reply
    Message reply;
    if (recv(sock, &reply, sizeof(Message), 0) < 0) {
        perror("Receive failed");
        return -1;
    }
    printf("Reply received successfully.\n");

    // Print reply details for debugging
    print_message_details(&reply);

    // Print reply bytes for debugging
    printf("Reply bytes:\n");
    print_bytes((uint8_t *)&reply, sizeof(Message));

    // Print the payload in the reply
    print_payload(reply.payload);

    // Check server response
    if (reply.message_type == 2 && strncmp(reply.payload, "ACCEPTED", 8) == 0) {
        printf("Server response: ACCEPTED\n");
    } else {
        printf("Server response: REJECTED\n");
    }

    // Close socket
    close(sock);
    return 0;
}
