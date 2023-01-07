#include "../include/message.h"
#include "../include/router.h"

oyamo_message_T *oyamo_message_create(char *name, char *operation)
{
    oyamo_message_T *message = malloc(sizeof(struct OYAMO_MESSAGE_STRUCT));
    message->name = name;
    message->operation = operation;
    message->parameters = json_object_new_object();
    message->status = OYAMO_JSON_SUCCESS;
    message->route_status = OYAMO_MESSAGE_ROUTE_STATUS_NO;

    return message;
}

oyamo_message_T *oyamo_message_create_success(char *name, char *operation, char *_message)
{
    oyamo_message_T *message = oyamo_message_create(name, operation);

    json_object_object_add(message->parameters, "success", json_object_new_string(_message));

    return message;
}

oyamo_message_T *oyamo_message_create_error(char *name, char *operation, char *error)
{
    oyamo_message_T *message = oyamo_message_create(name, operation);

    json_object_object_add(message->parameters, "error", json_object_new_string(error));

    return message;
}

oyamo_message_T *oyamo_message_create_same(oyamo_message_T *message)
{
    return oyamo_message_create(message->name, message->operation);
}

const char *oyamo_message_serialize(oyamo_message_T *message)
{
    json_object *root = json_object_new_object();
    json_object *route = json_object_new_object();
    json_object *parameters = json_object_new_object();

    if (message->name != NULL)
    {
        json_object_object_add(route, "name", json_object_new_string(message->name));
    }

    if (message->operation != NULL)
    {
        json_object_object_add(route, "operation", json_object_new_string(message->operation));
    }

    char route_status[2];
    sprintf(route_status, "%d", message->route_status);

    json_object_object_add(route, "status", json_object_new_string(route_status));

    json_object_object_add(root, "route", route);
    json_object_object_add(root, "parameters", message->parameters);

    return json_object_to_json_string(root);
}

void *oyamo_message_free(oyamo_message_T *message)
{
    message->name = NULL;
    message->operation = NULL;
    json_object_put(message->parameters);
    free(message);
}

oyamo_message_T *oyamo_message_from_json(oyamo_json_T *oyamo_json)
{
    oyamo_message_T *message = malloc(sizeof(struct OYAMO_MESSAGE_STRUCT));

    if (oyamo_json->status & OYAMO_JSON_INVALID)
    {
        oyamo_print_warning("[oyamo_message_from_json] OYAMO_JSON_INVALID");
        message->status = oyamo_json->status;
        message->parameters = json_object_new_object();

        return message;
    }

    struct json_object *json_route;
    struct json_object *json_route_name;
    struct json_object *json_route_operation;
    struct json_object *json_route_status;

    json_object_object_get_ex(oyamo_json->json, "route", &json_route);

    if (json_route == NULL)
    {
        message->status |= OYAMO_JSON_ERROR_MESSAGE_ROUTE;
    }
    else
    {
        json_object_object_get_ex(json_route, "name", &json_route_name);
        json_object_object_get_ex(json_route, "operation", &json_route_operation);
        json_object_object_get_ex(json_route, "status", &json_route_status);

        message->name = (char *)json_object_get_string(json_route_name);
        message->operation = (char *)json_object_get_string(json_route_operation);

        const char *route_status = json_object_get_string(json_route_status);

        if (route_status)
        {
            message->route_status = atoi(route_status);
        }
        else
        {
            message->route_status = OYAMO_MESSAGE_ROUTE_STATUS_NO;
        }
    }

    json_object_object_get_ex(oyamo_json->json, "parameters", &message->parameters);

    if (message->name == NULL)
    {
        oyamo_print_warning("[oyamo_message_from_json] OYAMO_JSON_ERROR_MESSAGE_NAME");
        message->status |= OYAMO_JSON_ERROR_MESSAGE_NAME;
        message->name = (char *)NULL;
    }

    if (message->operation == NULL)
    {
        oyamo_print_warning("[oyamo_message_from_json] OYAMO_JSON_ERROR_MESSAGE_OPERATION");
        message->status |= OYAMO_JSON_ERROR_MESSAGE_OPERATION;
        message->name = (char *)NULL;
    }

    if (message->parameters == NULL)
    {
        oyamo_print_warning("[oyamo_message_from_json] OYAMO_JSON_ERROR_MESSAGE_PARAMETERS");
        message->status |= OYAMO_JSON_ERROR_MESSAGE_PARAMETERS;
    }
    else
    {
        message->parameters = json_object_get(message->parameters);
    }

    if (!(message->status & (OYAMO_JSON_ERROR_MESSAGE_ROUTE | OYAMO_JSON_ERROR_MESSAGE_NAME | OYAMO_JSON_ERROR_MESSAGE_OPERATION | OYAMO_JSON_ERROR_MESSAGE_PARAMETERS)))
    {
        message->status |= OYAMO_JSON_SUCCESS;
    }

    return message;
}
