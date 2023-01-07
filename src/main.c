#include "../include/server.h"

void *onconnectTest(oyamo_server_client_T *client)
{
    oyamo_print_success("onConnect event called! %d", client->socket);
}

void onmessageTest(oyamo_server_client_T *client, oyamo_message_T* message)
{
    printf("called onmessage\n");
    printf("name: %s, op: %s\n", message->name, message->operation);
}

void ondisconnectTest(oyamo_server_client_T *client)
{
    oyamo_print_success("onDisconnect event called! %d", client->socket);
}


int main()
{
    oyamo_set_verbose(OYAMO_VERBOSE_ALL);
    oyamo_server_init();
    oyamo_server_set_config(oyamo_server_get_default_config("0.0.0.0", 8080));
    oyamo_server_event_set_onconnect(&onconnectTest);
    oyamo_server_event_set_ondisconnect(&ondisconnectTest);
    oyamo_server_event_set_onmessage(&onmessageTest);
    oyamo_server_go();
    oyamo_server_shutdown();

    return EXIT_SUCCESS;
}