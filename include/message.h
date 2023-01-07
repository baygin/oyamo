#ifndef OYAMO_MESSAGE_H
#define OYAMO_MESSAGE_H

#include "json.h"

#include <json-c/json.h>

typedef enum
{
    OYAMO_MESSAGE_ROUTE_STATUS_NO,
    OYAMO_MESSAGE_ROUTE_STATUS_SUCCESS,
    OYAMO_MESSAGE_ROUTE_STATUS_ERROR,
} oyamo_message_route_status;

typedef struct OYAMO_MESSAGE_STRUCT
{
    char *name;
    char *operation;
    struct json_object *parameters;
    unsigned short int status;
    oyamo_message_route_status route_status;
} oyamo_message_T;

oyamo_message_T *oyamo_message_from_json(oyamo_json_T *oyamo_json);
oyamo_message_T *oyamo_message_create(char *name, char *operation);
oyamo_message_T *oyamo_message_create_success(char *name, char *operation, char *message);
oyamo_message_T *oyamo_message_create_error(char *name, char *operation, char *error);
oyamo_message_T *oyamo_message_create_same(oyamo_message_T *message);
void *oyamo_message_free(oyamo_message_T *message);
const char *oyamo_message_serialize(oyamo_message_T *message);

#endif // !OYAMO_MESSAGE_H