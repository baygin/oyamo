#include "../include/server.h"
#include "../include/uuid.h"

/**
 * @brief The config keeps everything that need to start OYAMO server
 */
static oyamo_server_config_T *oyamo_server_config;

static oyamo_server_client_T **oyamo_server_clients;

static pthread_mutex_t oyamo_server_clients_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for oyamo_server_clients

static oyamo_events_struct_T *oyamo_server_events;

static int oyamo_server_socket; // Main socket file descriptor

static unsigned int oyamo_server_alive_client = 0;      // Alive total client
static unsigned int oyamo_server_last_client_index = 0; // all that created
static pthread_t oyamo_server_accept_thread;            // To create a thread for oyamo_server_accept function
static unsigned long oyamo_server_client_size;

/**
 * @brief Init OYAMO
 */
void oyamo_server_init()
{
    oyamo_server_config = malloc(sizeof(struct OYAMO_SERVER_CONFIG_STRUCT));
    oyamo_server_events = malloc(sizeof(struct OYAMO_EVENTS_STRUCT));
    oyamo_server_events = oyamo_events_init();
    oyamo_server_client_size = sizeof(struct OYAMO_SERVER_CLIENT_STRUCT);
}

/**
 * @brief To clear all variable that allocated
 */
void oyamo_server_shutdown()
{
    oyamo_print_info("Destroying OYAMO...");

    /**
     * @brief Kill all clients
     */
    for (size_t i = 0; i < oyamo_server_config->connection_limit; i++)
    {
        if (oyamo_server_clients[i])
        {
            oyamo_server_remove_client(oyamo_server_clients[i]);
        }
    }

    /**
     * @brief Free clients
     */
    free(oyamo_server_clients);

    /**
     * @brief Free router
     */

    oyamo_router_free(oyamo_server_config->router);

    /**
     * @brief Free server config
     */

    free(oyamo_server_config);

    /**
     * @brief Free events
     */
    oyamo_events_free(oyamo_server_events);

    /**
     * @brief If OYAMO running as a thread
     */
    oyamo_server_config->alive = false;
    shutdown(oyamo_server_socket, SHUT_RDWR);

    oyamo_print_success("OYAMO destroyed...");
}

/**
 * @brief Default OYAMO config
 */
oyamo_server_config_T *oyamo_server_get_default_config(char *address, unsigned int port)
{
    oyamo_server_config_T *config = malloc(sizeof(struct OYAMO_SERVER_CONFIG_STRUCT));

    config->address = address;
    config->port = port;
    config->buffer_size = 1024;
    config->connection_limit = (unsigned int)10;
    config->thread = false;
    config->alive = false;
    config->router = NULL;

    return config;
}

/**
 * @brief Get current OYAMO config
 */
oyamo_server_config_T *oyamo_server_get_config()
{
    return oyamo_server_config;
}

/**
 * @brief Set OYAMO config
 */
void oyamo_server_set_config(oyamo_server_config_T *_config)
{
    oyamo_server_config = _config;
    oyamo_server_clients = calloc(oyamo_server_config->connection_limit, oyamo_server_client_size);
}

/**
 * @brief Start OYAMO Server
 */
int oyamo_server_go()
{
    int reuse = 1;
    struct sockaddr_in oyamo_address;
    struct sockaddr_in client_address;

    // Create a socket for the server
    oyamo_server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Set up the server address structure
    oyamo_address.sin_family = AF_INET;
    oyamo_address.sin_port = htons(oyamo_server_config->port);
    oyamo_address.sin_addr.s_addr = inet_addr(oyamo_server_config->address);

    // Set the 'reuse' socket option
    if (setsockopt(oyamo_server_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), &reuse, sizeof(reuse)) < 0)
    {
        oyamo_print_error("'setsockopt()' Failed");
        return EXIT_FAILURE;
    }

    // Bind the socket to the server address
    if (bind(oyamo_server_socket, (struct sockaddr *)&oyamo_address, sizeof(oyamo_address)) < 0)
    {
        oyamo_print_error("'bind()' Failed");
        return EXIT_FAILURE;
    }

    // Start listening for connections
    if (listen(oyamo_server_socket, oyamo_server_config->connection_limit) < 0)
    {
        oyamo_print_error("'listen()' Failed");
        return EXIT_FAILURE;
    }

    // Print a success message
    oyamo_print_success("OYAMO started successfuly...");
    oyamo_print_success("Server running on [%s:%d]", oyamo_server_config->address, oyamo_server_config->port);

    // If the 'thread' configuration option is set, create a thread to handle connections
    if (oyamo_server_config->thread)
    {
        if (pthread_create(&oyamo_server_accept_thread, NULL, (void *)oyamo_server_accept, NULL))
        {
            oyamo_print_error("Could not create the accept thread!");
            oyamo_server_shutdown();
        }

        // Detach the thread
        pthread_detach(oyamo_server_accept_thread);
    }
    // Otherwise, handle connections in the main thread
    else
    {
        oyamo_server_accept();
    }

    // Return success

    return EXIT_SUCCESS;
}

static void oyamo_server_add_client(oyamo_server_client_T *client)
{
    for (size_t i = 0; i < oyamo_server_config->connection_limit; i++)
    {
        if (oyamo_server_clients[i] == NULL)
        {
            oyamo_server_clients[i] = client;
            break;
        }
    }
}

static unsigned short int oyamo_server_remove_client(oyamo_server_client_T *client)
{
    for (size_t i = 0; i < oyamo_server_config->connection_limit; i++)
    {
        if (oyamo_server_clients[i] && oyamo_server_clients[i]->socket == client->socket)
        {
            pthread_mutex_lock(&client->mutex);
            oyamo_server_clients[i] = NULL;
            pthread_mutex_unlock(&client->mutex);
            pthread_mutex_destroy(&client->mutex);

            shutdown(client->socket, SHUT_RDWR);
            close(client->socket);

            pthread_join(client->thread, NULL);

            free(client->attr);
            free(client);

            oyamo_server_alive_client--;

            return 0;

            break;
        }
    }
    
    return 1;
}

static void oyamo_server_client_listen(oyamo_server_client_T *client)
{
    char *buffer = malloc(oyamo_server_config->buffer_size * sizeof(char));
    oyamo_server_alive_client++;
    oyamo_server_last_client_index++;

    oyamo_server_event_run_onconnect(client);

    oyamo_print_white("[%s:%d] has joined", client->sin_address, client->sin_port);

    while (oyamo_server_config->alive && client->alive)
    {
        int receive = recv(client->socket, buffer, oyamo_server_config->buffer_size, 0);

        if (receive > 0)
        {
            oyamo_json_T *json = oyamo_json_from_string((char *)buffer);

            if (json->status & OYAMO_JSON_SUCCESS)
            {
                oyamo_message_T *message = oyamo_message_from_json(json);

                if (message->status & OYAMO_JSON_SUCCESS)
                {
                    oyamo_server_event_run_onmessage(client, message);

                    if (oyamo_server_config->router)
                    {
                        oyamo_router_resolve_router(oyamo_server_config->router, client, message);
                    }
                }

                oyamo_message_free(message);
            }

            json_object_put(json->json);
            free(json);
        }
        else if (receive == 0)
        {
            client->alive = false;
        }
    }

    free(buffer);

    oyamo_print_error("[%s:%d] has left", client->sin_address, client->sin_port);

    oyamo_server_event_run_ondisconnect(client);

    oyamo_server_remove_client(client);
}

// Writing alive count to the terminal in every 1 second

static void oyamo_server_write_alive_client()
{
    while (1)
    {
        oyamo_print_info("Alive: %d", oyamo_server_alive_client);
        sleep(1);
    }
}

static void oyamo_server_accept()
{
    oyamo_server_config->alive = true;

    oyamo_print_success("Listening...");

    struct sockaddr_in client_address;
    pthread_t client_alive_thread;
    int client_socket;
    int client_length;

    client_length = sizeof(struct sockaddr_in);

    // pthread_create(&client_alive_thread, NULL, (void *)oyamo_server_write_alive_client, NULL);

    while (oyamo_server_config->alive)
    {
        client_socket = accept(oyamo_server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_length);

        if (!oyamo_server_config->alive)
        {
            break;
        }

        if (client_socket < 0)
        {
            oyamo_print_error("Error on accepting connections, error no: %d", errno);
            continue;
        }

        char *client_sin_address = inet_ntoa(client_address.sin_addr);
        uint16_t client_sin_port = ntohs(client_address.sin_port);

        if (oyamo_server_alive_client + 1 == oyamo_server_config->connection_limit)
        {
            oyamo_print_warning("[%s:%d] Alive clients reached to permitted count [%d]", client_sin_address, client_sin_port, oyamo_server_config->connection_limit);
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
            continue;
        }

        oyamo_print_success("[%s:%d] Client accepted", client_sin_address, client_sin_port);

        oyamo_server_client_T *client = (oyamo_server_client_T *)malloc(oyamo_server_client_size);
        client->socket = client_socket;
        client->sin_address = client_sin_address;
        client->sin_port = client_sin_port;
        client->alive = true;
        client->uuid = oyamo_generate_uuid();
        pthread_mutex_init(&client->mutex, NULL);

        oyamo_server_add_client(client);

        if (pthread_create(&client->thread, NULL, (void *)oyamo_server_client_listen, (oyamo_server_client_T *)client))
        {
            oyamo_print_error("Could not create the client thread!");
            oyamo_server_remove_client(client);
            continue;
        }

        pthread_detach(client->thread);
    }

    oyamo_print_success("Accept thread killed.");

    pthread_exit(NULL);
}

void oyamo_server_send_message_to_all(oyamo_message_T *message, oyamo_server_client_T *except)
{
    for (size_t i = 0; i < oyamo_server_config->connection_limit; i++)
    {
        if (oyamo_server_clients[i])
        {
            if (except == NULL)
            {
                oyamo_server_send_message_to_client(message, oyamo_server_clients[i]);
            }
            else if (oyamo_server_clients[i]->socket != except->socket)
            {
                oyamo_server_send_message_to_client(message, oyamo_server_clients[i]);
            }
        }
    }
}

int oyamo_server_send_message_to_client(oyamo_message_T *message, oyamo_server_client_T *client)
{
    if (!message || (message && message->status & OYAMO_JSON_INVALID))
    {
        return 1;
    }

    const char *serialized = oyamo_message_serialize(message);
    size_t length = strlen(serialized);

    pthread_mutex_lock(&client->mutex);

    if (!client->socket)
    {
        return 1;
    }

    if (send(client->socket, serialized, length, 0) < 0)
    {
        oyamo_print_error("message send error\n");
        pthread_mutex_unlock(&client->mutex);
        oyamo_server_remove_client(client);
        return 1;
    }

    pthread_mutex_unlock(&client->mutex);

    return 0;
}

// event

void oyamo_server_event_set_onconnect(void *function)
{
    oyamo_event_set_function(oyamo_server_events->onconnect, function);
}

void oyamo_server_event_set_onmessage(void *function)
{
    oyamo_event_set_function(oyamo_server_events->onmessage, function);
}

void oyamo_server_event_set_ondisconnect(void *function)
{
    oyamo_event_set_function(oyamo_server_events->ondisconnect, function);
}

static void oyamo_server_event_run_onconnect(oyamo_server_client_T *client)
{
    oyamo_event_run(oyamo_server_events->onconnect, client, (oyamo_message_T *)NULL);
}

static void oyamo_server_event_run_onmessage(oyamo_server_client_T *client, oyamo_message_T *message)
{
    oyamo_event_run(oyamo_server_events->onmessage, client, message);
}

static void oyamo_server_event_run_ondisconnect(oyamo_server_client_T *client)
{
    oyamo_event_run(oyamo_server_events->ondisconnect, client, (oyamo_message_T *)NULL);
}

void oyamo_server_set_client_attr(oyamo_server_client_T *client, void *attr, size_t size)
{
    client->attr = malloc(size);
    client->attr = attr;
}

void oyamo_server_set_router(oyamo_router_T *router)
{
    if (oyamo_server_config->router == NULL)
    {
        oyamo_server_config->router = malloc(sizeof(struct OYAMO_ROUTER_STRUCT));
    }

    oyamo_server_config->router = router;
}
