#include "../include/json.h"
#include "../include/router.h"

oyamo_router_T *oyamo_router_create()
{
    oyamo_router_T *router = malloc(sizeof(struct OYAMO_ROUTER_STRUCT));
    router->length = 0;
    router->routes = calloc(router->length, sizeof(struct OYAMO_ROUTER_ROUTE_STRUCT));

    return router;
}

void oyamo_router_free(oyamo_router_T *router)
{
    for (size_t i = 0; i < router->length; i++)
    {
        if (router->routes[i])
        {
            oyamo_router_free_route(router->routes[i]);
        }
    }

    free(router->routes);
    free(router);
}

static void oyamo_router_free_route(oyamo_router_route_T *route)
{
    free(route->functions);
    free(route->middleware->functions);
    free(route->middleware);
    free(route);
}

oyamo_router_route_T *oyamo_router_create_route(const char *root_name, const char *operation_name, void *function)
{
    oyamo_router_route_T *route = malloc(sizeof(struct OYAMO_ROUTER_ROUTE_STRUCT));
    route->name = (char *)root_name;
    route->operation = (char *)operation_name;
    route->functions = malloc(sizeof(struct OYAMO_ROUTER_FUNCTION_STRUCT));
    route->functions->function = function;

    route->middleware = malloc(sizeof(struct OYAMO_ROUTER_MIDDLEWARE_STRUCT));
    route->middleware->length = 0;
    route->middleware->functions = calloc(route->middleware->length, sizeof(void *));

    return route;
}

void oyamo_router_set_route_onsuccess(oyamo_router_route_T *route, void *function)
{
    route->functions->onsuccess = function;
}

void oyamo_router_set_route_onerror(oyamo_router_route_T *route, void *function)
{
    route->functions->onerror = function;
}

int oyamo_router_resolve_router(oyamo_router_T *router, void *client, oyamo_message_T *message)
{
    short int middleware_break = 0;

    if (message->status & OYAMO_JSON_INVALID)
    {
        oyamo_print_warning("[oyamo_router_resolve_router] OYAMO_JSON_INVALID");
        return OYAMO_JSON_INVALID;
    }

    for (size_t i = 0; i < router->length; i++)
    {
        if (router->routes[i] == NULL)
        {
            continue;
        }

        if (router->routes[i]->functions == NULL)
        {
            oyamo_print_warning("[oyamo_router_resolve] there is no route functions in route");
            continue;
        }
        else if (router->routes[i]->functions->function == NULL)
        {
            oyamo_print_warning("[oyamo_router_resolve] there is no function in route");
            continue;
        }

        if (message->name == NULL)
        {
            oyamo_print_warning("[oyamo_router_resolve] there is no name in message");
            continue;
        }

        if (message->operation == NULL)
        {
            oyamo_print_warning("[oyamo_router_resolve] there is no operation in message");
            continue;
        }

        if (strcmp(router->routes[i]->name, message->name) == 0 && strcmp(router->routes[i]->operation, message->operation) == 0)
        {
            for (int mi = 0; mi < router->routes[i]->middleware->length; mi++)
            {
                if (router->routes[i]->middleware->functions[mi] == NULL)
                {
                    continue;
                }

                if ((middleware_break = router->routes[i]->middleware->functions[mi](client, message)))
                {
                    return middleware_break;
                }
            }

            router->routes[i]->functions->function(client, message);

            if (message->route_status == OYAMO_MESSAGE_ROUTE_STATUS_SUCCESS && router->routes[i]->functions->onsuccess)
            {
                router->routes[i]->functions->onsuccess(client, message);
            }

            if (message->route_status == OYAMO_MESSAGE_ROUTE_STATUS_ERROR && router->routes[i]->functions->onsuccess)
            {
                router->routes[i]->functions->onsuccess(client, message);
            }
        }
    }

    return 0;
}

void oyamo_router_set_middleware(oyamo_router_route_T *route, void *function)
{
    route->middleware->length++;
    route->middleware->functions = realloc(route->middleware->functions, route->middleware->length * sizeof(void *));
    route->middleware->functions[route->middleware->length - 1] = function;
}

int oyamo_router_set_route(oyamo_router_T *router, oyamo_router_route_T *route)
{
    if (route->name == NULL)
    {
        oyamo_print_error("[oyamo_router_set] route name is wrong");
        return 1;
    }

    if (route->operation == NULL)
    {
        oyamo_print_error("[oyamo_router_set] route operation is wrong");
        return 1;
    }
    if (!route->functions->function && !route->functions->onsuccess && !route->functions->onerror)
    {
        oyamo_print_error("[oyamo_router_set] route functions is wrong");
        return 1;
    }

    router->length++;
    router->routes = realloc(router->routes, router->length * sizeof(struct OYAMO_ROUTER_ROUTE_STRUCT));
    router->routes[router->length - 1] = route;

    return 0;
}