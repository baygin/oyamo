#include "../include/client.h"

static oyamo_client_config_T *oyamo_client_config;
static oyamo_events_struct_T *oyamo_client_events;

void oyamo_client_init()
{
    oyamo_client_config = malloc(sizeof(struct OYAMO_CLIENT_CONFIG_STRUCT));
    oyamo_client_events = malloc(sizeof(struct OYAMO_EVENTS_STRUCT));
    oyamo_client_events = oyamo_events_init();
}

oyamo_client_config_T *oyamo_client_get_config()
{
    return oyamo_client_config;
}

static int oyamo_client_socket;
static pthread_t oyamo_client_receive_thread;

/**
 * @brief Start OYAMO Server
 */
int oyamo_client_go()
{
    oyamo_client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in oyamo_address;

    if (oyamo_client_socket == -1)
    {
        oyamo_print_error("[oyamo_client_go] couldn't create the socket");
        return EXIT_FAILURE;
    }

    oyamo_address.sin_family = AF_INET;
    oyamo_address.sin_port = htons(oyamo_client_config->port);
    oyamo_address.sin_addr.s_addr = inet_addr(oyamo_client_config->address);

    if (connect(oyamo_client_socket, (struct sockaddr *)&oyamo_address, sizeof(oyamo_address)) != 0)
    {
        oyamo_print_error("[oyamo_client_go] couldn't connect to the server");
        return EXIT_FAILURE;
    }

    oyamo_print_success("successfully connected to %s:%d", oyamo_client_config->address, oyamo_client_config->port);

    oyamo_client_config->alive = true;

    if (oyamo_client_config->thread)
    {
        if (pthread_create(&oyamo_client_receive_thread, NULL, (void *)oyamo_client_receive, NULL))
        {
            oyamo_print_error("couldn't create the receive thread!");
            oyamo_client_shutdown();
        }
    }
    else
    {
        oyamo_client_receive();
    }

    return EXIT_SUCCESS;
}

void oyamo_client_receive()
{
    char buffer[oyamo_client_config->buffer_size];

    while (oyamo_client_config->alive)
    {
        int receive = recv(oyamo_client_socket, buffer, sizeof(buffer), 0);

        if (receive > 0)
        {
            oyamo_json_T *json = oyamo_json_from_string((char *)buffer);

            if (json->status & OYAMO_JSON_SUCCESS)
            {
                oyamo_message_T *message = oyamo_message_from_json(json);

                if (message && message->status & OYAMO_JSON_SUCCESS)
                {
                    oyamo_client_event_run_onmessage(message);

                    if (oyamo_client_config->router)
                    {
                        oyamo_router_resolve_router(oyamo_client_config->router, (void *)0, message);
                    }
                }

                oyamo_message_free(message);
            }
            else
            {
                oyamo_print_warning("[oyamo_client_receive] message is not a json");
            }

            json_object_put(json->json);
            free(json);
        }
        else
        {
            oyamo_client_config->alive = false;
        }

        bzero(buffer, oyamo_client_config->buffer_size);
    }

    oyamo_print_error("[oyamo_client_receive] connection lost");
}

int oyamo_client_send(oyamo_message_T *message)
{
    const char *serialized = oyamo_message_serialize(message);

    size_t length = strlen(serialized);

    if (send(oyamo_client_socket, serialized, length, 0) < 0)
    {
        oyamo_print_error("message send error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void oyamo_client_shutdown()
{
    oyamo_client_config->alive = false;
    oyamo_client_config->router;

    oyamo_events_free(oyamo_client_events);
    free(oyamo_client_config);
}

oyamo_client_config_T *oyamo_client_get_default_config(char *address, unsigned int port)
{
    oyamo_client_config_T *config = malloc(sizeof(struct OYAMO_CLIENT_CONFIG_STRUCT));
    config->address = address;
    config->port = port;
    config->buffer_size = 1024;
    config->router = NULL;
    config->thread = false;
    config->alive = false;

    return config;
}

void oyamo_client_set_config(oyamo_client_config_T *config)
{
    if (oyamo_client_config == NULL)
    {
        oyamo_client_config = malloc(sizeof(struct OYAMO_CLIENT_CONFIG_STRUCT));
    }

    oyamo_client_config = config;
}

void oyamo_client_set_router(oyamo_router_T *router)
{
    if (oyamo_client_config->router == NULL)
    {
        oyamo_client_config->router = malloc(sizeof(struct OYAMO_ROUTER_STRUCT));
    }

    oyamo_client_config->router = router;
}

void oyamo_client_event_set_onconnect(void *function)
{
    oyamo_event_set_function(oyamo_client_events->onconnect, function);
}

void oyamo_client_event_set_onmessage(void *function)
{
    oyamo_event_set_function(oyamo_client_events->onmessage, function);
}

void oyamo_client_event_set_ondisconnect(void *function)
{
    oyamo_event_set_function(oyamo_client_events->ondisconnect, function);
}

static void oyamo_client_event_run_onconnect()
{
    oyamo_event_run(oyamo_client_events->onconnect, (void *)0, (void *)0);
}

static void oyamo_client_event_run_onmessage(oyamo_message_T *message)
{
    oyamo_event_run(oyamo_client_events->onmessage, (void *)0, message);
}

static void oyamo_client_event_run_ondisconnect()
{
    oyamo_event_run(oyamo_client_events->ondisconnect, (void *)0, (void *)0);
}