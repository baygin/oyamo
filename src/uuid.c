#include "../include/uuid.h"

char *oyamo_generate_uuid()
{
    uuid_t uuid;
    
    // Allocate memory for the UUID string
    char *uuid_string = malloc(37 * sizeof(char));

    // Generate a UUID
    uuid_generate_time_safe(uuid);
    
    // Convert the UUID to a string
    uuid_unparse_lower(uuid, uuid_string);

    // Return the UUID string
    return uuid_string;
}
