#ifndef OYAMO_JSON_H
#define OYAMO_JSON_H

#include "io.h"

#include <string.h>
#include <json-c/json.h>

#define OYAMO_JSON_SUCCESS (1 << 0)
#define OYAMO_JSON_INVALID (1 << 1)
#define OYAMO_JSON_ERROR_MESSAGE_ROUTE (1 << 2)
#define OYAMO_JSON_ERROR_MESSAGE_NAME (1 << 3)
#define OYAMO_JSON_ERROR_MESSAGE_OPERATION (1 << 4)
#define OYAMO_JSON_ERROR_MESSAGE_PARAMETER (1 << 5)
#define OYAMO_JSON_ERROR_MESSAGE_PARAMETERS (1 << 6)

typedef struct OYAMO_JSON_STRUCT
{
    struct json_object *json;
    unsigned short int status;
} oyamo_json_T;

oyamo_json_T *oyamo_json_from_string(char *json_text);

#endif // !OYAMO_JSON_H