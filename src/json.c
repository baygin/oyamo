#include "../include/json.h"

oyamo_json_T *oyamo_json_from_string(char *json_text)
{
    // Allocate memory for an 'oyamo_json_T' structure
    oyamo_json_T *oyamo_json = malloc(1 * sizeof(struct OYAMO_JSON_STRUCT));

    // Parse the JSON text and store the result in the 'oyamo_json->json' field
    oyamo_json->json = json_tokener_parse(json_text);

    // If the parse failed, set the 'OYAMO_JSON_INVALID' status flag
    if (oyamo_json->json == NULL)
    {
        oyamo_print_warning("[oyamo_json_from_string] OYAMO_JSON_INVALID");
        oyamo_json->status |= OYAMO_JSON_INVALID;
    }
    // Otherwise, set the 'OYAMO_JSON_SUCCESS' status flag
    else
    {
        oyamo_json->status |= OYAMO_JSON_SUCCESS;
    }

    // Return the 'oyamo_json_T' structure
    return oyamo_json;
}