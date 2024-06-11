#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"

#define SERVER_ADDRESS "3.91.129.142"
#define SERVER_PORT 8162
#define PAYLOAD_MAX_LENGTH 64
#define BUFFER_SIZE 128  // Define a buffer size

#define VERBOSE 0

typedef enum {
    STATE_INIT,
    STATE_SEND,
    STATE_RECEIVE,
    STATE_IDLE,
    STATE_ERROR
} State;

void create_message(uint8_t *buffer, uint8_t *counter, const char *payload, uint16_t *message_length) {
    uint16_t magic = 0xf00d;
    uint16_t length = strlen(payload);
    uint8_t message_type = 1;
    uint64_t timestamp = (uint64_t)time(NULL);
    uint16_t payload_cs = calculate_checksum((uint8_t *)payload, length);
    uint16_t header_cs;
    
    // Fill the buffer
    int offset = 0;
    memcpy(buffer + offset, &magic, sizeof(magic));
    offset += sizeof(magic);
    memcpy(buffer + offset, &length, sizeof(length));
    offset += sizeof(length);
    memcpy(buffer + offset, &message_type, sizeof(message_type));
    offset += sizeof(message_type);
    memcpy(buffer + offset, &timestamp, sizeof(timestamp));
    offset += sizeof(timestamp);
    memcpy(buffer + offset, counter, sizeof(*counter));
    offset += sizeof(*counter);
    memcpy(buffer + offset, &payload_cs, sizeof(payload_cs));
    offset += sizeof(payload_cs);

    // Calculate header checksum
    header_cs = calculate_checksum(buffer, offset);
    memcpy(buffer + offset, &header_cs, sizeof(header_cs));
    offset += sizeof(header_cs);

    // Copy payload to buffer
    memcpy(buffer + offset, payload, length);
    offset += length;

    *message_length = offset;
    (*counter)++;

#if VERBOSE
    // Print the message being sent
    printf("Sending message: ");
    for (int i = 0; i < *message_length; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");
#endif
}

void send_message(const uint8_t *buffer, size_t length, int sockfd) {
    send(sockfd, buffer, length, 0);
}

void receive_response(int sockfd) {
    char response[BUFFER_SIZE];
    int response_length = recv(sockfd, response, sizeof(response) - 1, 0);
    if (response_length < 0) {
        perror("recv failed");
        return;
    }
    response[response_length] = '\0';  // Add null byte

#if VERBOSE
    // Print the response
    printf("Received response: ");
    for (int i = 0; i < response_length; i++) {
        printf("%02x ", (unsigned char)response[i]);
    }
    printf("\n");

    // Break down the response
    printf("Response breakdown:\n");
    printf("Magic: %02x %02x\n", (unsigned char)response[0], (unsigned char)response[1]);
    printf("Length: %02x %02x\n", (unsigned char)response[2], (unsigned char)response[3]);
    printf("Message type: %02x\n", (unsigned char)response[4]);
    printf("Timestamp: %02x %02x %02x %02x %02x %02x %02x %02x\n", (unsigned char)response[5], (unsigned char)response[6], (unsigned char)response[7], (unsigned char)response[8], (unsigned char)response[9], (unsigned char)response[10], (unsigned char)response[11], (unsigned char)response[12]);
    printf("Counter: %02x\n", (unsigned char)response[13]);
    printf("Payload checksum: %02x %02x\n", (unsigned char)response[14], (unsigned char)response[15]);
    printf("Header checksum: %02x %02x\n", (unsigned char)response[16], (unsigned char)response[17]);

    // Add this loop to print the payload in ASCII

    
#endif
    printf("Payload from Server: ");
    for (int i = 18; i < response_length; i++) {
        printf("%02x ", (unsigned char)response[i]);
    }
    printf("\n");

    printf("Payload from Server (ASCII): ");
    for (int i = 18; i < response_length; i++) {
        printf("%c", response[i]);
    }
    printf("\n");
}

int init_connection(int *sockfd, struct sockaddr_in *server_addr) {
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_addr->sin_addr) <= 0) {
        perror("Invalid address");
        close(*sockfd);
        return -1;
    }
    if (connect(*sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("Connection failed");
        close(*sockfd);
        return -1;
    }
    return 0;
}

void close_connection(int sockfd) {
    close(sockfd);
}

void StateMachine(State *current_state, int *sockfd, struct sockaddr_in *server_addr, uint8_t *buffer, uint8_t *counter, char *payload, uint16_t *message_length) {
    switch (*current_state) {
        case STATE_INIT:
            if (init_connection(sockfd, server_addr) == 0) {
                *current_state = STATE_SEND;
            } else {
                *current_state = STATE_ERROR;
            }
            break;

        case STATE_SEND:
            create_message(buffer, counter, payload, message_length);
            send_message(buffer, *message_length, *sockfd);
            *current_state = STATE_RECEIVE;
            break;

        case STATE_RECEIVE:
            receive_response(*sockfd);
            *current_state = STATE_IDLE;
            break;

        case STATE_IDLE:
            close_connection(*sockfd);
            *current_state = STATE_INIT;
            break;

        case STATE_ERROR:
            // Handle errors
            printf("An error occurred. Exiting.\n");
            close(*sockfd);
            exit(EXIT_FAILURE);
            break;

        default:
            *current_state = STATE_ERROR;
            break;
    }
}

int main() {
    uint8_t buffer[BUFFER_SIZE];
    uint8_t counter = 0;
    char payload[PAYLOAD_MAX_LENGTH];
    uint16_t message_length;
    int sockfd;
    struct sockaddr_in server_addr;
    State current_state = STATE_INIT;

    printf("Enter the name to send: ");
    char name[PAYLOAD_MAX_LENGTH - 2];  // Subtract 2 for the brackets
    scanf("%s", name);
    snprintf(payload, PAYLOAD_MAX_LENGTH, "[%s]", name);

    while (current_state != STATE_IDLE) { //This would be assumed to run on the HB and only trigger when commanded. 
        StateMachine(&current_state, &sockfd, &server_addr, buffer, &counter, payload, &message_length);
    }

    return 0;
}
