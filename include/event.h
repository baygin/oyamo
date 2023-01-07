#ifndef OYAMO_EVENT_H
#define OYAMO_EVENT_H

#include "message.h"

#include <stddef.h>
#include <stdlib.h>

/**
 * @brief OYAMO event
 */
typedef struct OYAMO_EVENT_STRUCT
{
    void (**functions)();
    size_t length;
} oyamo_event_struct_T;

/**
 * @brief OYAMO events for onconnect, ondisconnect and onmessage
 */
typedef struct OYAMO_EVENTS_STRUCT
{
    oyamo_event_struct_T *onconnect;
    oyamo_event_struct_T *ondisconnect;
    oyamo_event_struct_T *onmessage;
} oyamo_events_struct_T;

oyamo_events_struct_T *oyamo_events_init();
void oyamo_events_free(oyamo_events_struct_T *events);
void oyamo_event_set_function(oyamo_event_struct_T *oyamo_event, void *function);
void oyamo_event_run(oyamo_event_struct_T *oyamo_event, void *client, oyamo_message_T* message);

#endif // !OYAMO_EVENT_H