#ifndef OYAMO_SERVER_H
#define OYAMO_SERVER_H

#include "message.h"
#include "event.h"
#include "io.h"
#include "router.h"
#include "json.h"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

typedef struct OYAMO_SERVER_CLIENT_STRUCT
{
    void *id;
    char *uuid;
    int socket;
    char *sin_address; // printable
    uint16_t sin_port; // printable
    pthread_t thread;
    bool alive;
    void *attr; // additional attributes
    pthread_mutex_t mutex;

} oyamo_server_client_T;

typedef struct OYAMO_SERVER_CONFIG_STRUCT
{
    char *address;                 // "0.0.0.0"
    unsigned int port;             // 8099
    unsigned int connection_limit; // 1024
    unsigned int buffer_size;      // 1024

    oyamo_router_T *router;

    bool thread;
    bool alive;
} oyamo_server_config_T;

oyamo_server_config_T *oyamo_server_get_default_config(char *address, unsigned int port);
oyamo_server_config_T *oyamo_server_get_config();
void oyamo_server_set_config(oyamo_server_config_T *_config);
void oyamo_server_set_router(oyamo_router_T *router);
void oyamo_server_init();
void oyamo_server_shutdown();
int oyamo_server_go();
static void oyamo_server_accept();
static void oyamo_server_client_listen(oyamo_server_client_T *client);
static void oyamo_server_write_alive_client();
static void oyamo_server_add_client(oyamo_server_client_T *client);
static unsigned short int oyamo_server_remove_client(oyamo_server_client_T *client_sock);
void oyamo_server_send_message_to_all(oyamo_message_T *message, oyamo_server_client_T *except);
int oyamo_server_send_message_to_client(oyamo_message_T *message, oyamo_server_client_T *client);
void oyamo_server_event_set_onconnect(void *function);
void oyamo_server_event_set_onmessage(void *function);
void oyamo_server_event_set_ondisconnect(void *function);
static void oyamo_server_event_run_onconnect(oyamo_server_client_T *client);
static void oyamo_server_event_run_onmessage(oyamo_server_client_T *client, oyamo_message_T *message);
static void oyamo_server_event_run_ondisconnect(oyamo_server_client_T *client);
static void oyamo_server_ping_all_client();
void oyamo_server_set_client_attr(oyamo_server_client_T *client, void *attr, size_t size);

#endif // !OYAMO_SERVER_H