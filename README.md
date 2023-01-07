# OYAMO - A Lightweight TCP/IP Library for C

OYAMO is a lightweight library for developing server-client applications using TCP/IP in C. It provides a simple and API for creating and managing connections, sending and receiving data, and handling events.

# Prerequisites

- gcc, clang, or another C compiler
- Make
- libuuid1
- json-c

## Features

- Easy-to-use API for creating and managing connections
- Support for multiple client connections
- Support for threading
- JSON support for sending structured data
- Custom event system for handling connections, disconnections, and incoming messages

## Example Server

Here is a simple example of a server application using OYAMO:

```c
#include <oyamo/server.h>

void on_connect(void *client)
{
    oyamo_message_T *message = oyamo_message_create("route", "operation");
    json_object_object_add(message->parameters, "message", json_object_new_string("Hello from the OYAMO server!"));

    oyamo_server_send_message_to_client(message, client);
}

int main()
{
    // Enable verbose logging
    oyamo_set_verbose(OYAMO_VERBOSE_ALL);

    // Initialize the OYAMO server
    oyamo_server_init();

    // Set the server configuration
    oyamo_server_config_T *config = oyamo_server_get_default_config("0.0.0.0", 8031);
    config->address = "0.0.0.0";
    config->port = 8031;
    config->connection_limit = 10;
    config->buffer_size = 1024;
    config->thread = false;
    oyamo_server_set_config(config);

    // Set the 'onconnect' event handler
    oyamo_server_event_set_onconnect(on_connect);

    // Start the OYAMO server
    return oyamo_server_go();
}
```

## Compile

```bash
$ gcc main.c -loyamo -luuid -ljson-c
```

## Other Examples

- <a href="https://github.com/baygin/oyamo-chat-server/" target="_blank"> Chat Server Implementation </a>
- <a href="https://github.com/baygin/oyamo-chat-client/" target="_blank"> Chat Client Implementation </a>

## Installation

To install OYAMO, you can follow these steps:

1. Update your package manager's list of available packages:

```bash
$ sudo apt update
```

2. Upgrade any installed packages that have updates available:

```bash
$ sudo apt upgrade -y
```

3. Clone the OYAMO repository:

```bash
$ git clone https://github.com/baygin/oyamo.git
```

4. Change into the OYAMO directory:

```bash
$ cd oyamo
```

5. Install the required dependencies:

```bash
$ make install-dependencies
```

6. Build the OYAMO library:

```bash
$ make
```

7. (Optional) Install OYAMO on your system:

```bash
$ sudo make install
```

## Contributing

If you want to contribute to the development of OYAMO, you can open an issue or submit a pull request.

## License

OYAMO is licensed under the GPL3. See <a href="https://github.com/baygin/oyamo/blob/master/LICENSE" target="blank"> LICENSE </a> for more information.
