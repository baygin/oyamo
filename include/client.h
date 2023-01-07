#ifndef OYAMO_CLIENT_H
#define OYAMO_CLIENT_H

// OYAMO Headers
#include "router.h"
#include "message.h"
#include "event.h"
#include "json.h"
#include "io.h"

#include <stdbool.h>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <pthread.h>

typedef struct OYAMO_CLIENT_CONFIG_STRUCT
{
    char *address;            // "0.0.0.0"
    unsigned int port;        // 8099
    unsigned int buffer_size; // 1024

    oyamo_router_T *router;

    bool thread;
    bool alive;
} oyamo_client_config_T;

void oyamo_client_init();
int oyamo_client_go();
void oyamo_client_receive();
int oyamo_client_send(oyamo_message_T *message);
void oyamo_client_shutdown();
oyamo_client_config_T *oyamo_client_get_config();
oyamo_client_config_T *oyamo_client_get_default_config(char *address, unsigned int port);
void oyamo_client_set_config(oyamo_client_config_T *config);
void oyamo_client_set_router();
void oyamo_client_event_set_onconnect(void *function);
void oyamo_client_event_set_onmessage(void *function);
void oyamo_client_event_set_ondisconnect(void *function);
static void oyamo_client_event_run_onconnect();
static void oyamo_client_event_run_onmessage(oyamo_message_T *message);
static void oyamo_client_event_run_ondisconnect();

#endif // !OYAMO_CLIENT_H
