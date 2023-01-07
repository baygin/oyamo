#ifndef OYAMO_ROUTER_H
#define OYAMO_ROUTER_H

#include "io.h"
#include "message.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct OYAMO_ROUTER_FUNCTION_STRUCT
{
    short int (*function)(void *client, oyamo_message_T *message);
    short int (*onsuccess)(void *client, oyamo_message_T *message);
    short int (*onerror)(void *client, oyamo_message_T *message);
} oyamo_router_function_T;

typedef struct OYAMO_ROUTER_MIDDLEWARE_STRUCT
{
    short int (**functions)(void *client, oyamo_message_T *message);
    size_t length;
} oyamo_router_middleware_T;

typedef struct OYAMO_ROUTER_ROUTE_STRUCT
{
    const char *name;
    const char *operation;
    oyamo_router_function_T *functions;
    oyamo_router_middleware_T *middleware;
} oyamo_router_route_T;

typedef struct OYAMO_ROUTER_STRUCT
{
    oyamo_router_route_T **routes;
    size_t length;
} oyamo_router_T;

oyamo_router_T *oyamo_router_create();
void oyamo_router_free(oyamo_router_T * router);
static void oyamo_router_free_route(oyamo_router_route_T* route);
int oyamo_router_resolve_router(oyamo_router_T *router, void *client,  oyamo_message_T *message);
oyamo_router_route_T *oyamo_router_create_route(const char *root_name, const char *operation_name, void *function);
void oyamo_router_set_route_onsuccess(oyamo_router_route_T *route, void *function);
void oyamo_router_set_route_onerror(oyamo_router_route_T *route, void *function);
int oyamo_router_set_route(oyamo_router_T *router, oyamo_router_route_T *route);
void oyamo_router_set_middleware(oyamo_router_route_T *route, void *function);

#endif // !OYAMO_ROUTER_H