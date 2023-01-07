#include "../include/event.h"

oyamo_events_struct_T *oyamo_events_init()
{
    size_t event_size = sizeof(struct OYAMO_EVENT_STRUCT);
    size_t void_size = sizeof(void **);

    // Initialize the events structure
    oyamo_events_struct_T *events = malloc(sizeof(struct OYAMO_EVENTS_STRUCT));

    // Allocate memory for the 'onconnect', 'ondisconnect', and 'onmessage' events
    events->onconnect = malloc(event_size);
    events->ondisconnect = malloc(event_size);
    events->onmessage = malloc(event_size);

    // Initialize the lengths of the events to 0
    events->onconnect->length = 0;
    events->ondisconnect->length = 0;
    events->onmessage->length = 0;

    // Return the events structure
    return events;
}

void oyamo_event_set_function(oyamo_event_struct_T *oyamo_event, void *function)
{
    // If the event has no functions, allocate memory for the functions array
    if (oyamo_event->length == 0)
    {
        oyamo_event->functions = calloc(oyamo_event->length + 1, sizeof(void **));
    }
    // Otherwise, reallocate memory for the functions array
    else
    {
        oyamo_event->functions = realloc(oyamo_event->functions, (oyamo_event->length + 1) * sizeof(void **));
    }

    // Add the function
    oyamo_event->functions[oyamo_event->length] = function;

    // Increase the functions length
    oyamo_event->length++;
}

void oyamo_event_run(oyamo_event_struct_T *oyamo_event, void *client, oyamo_message_T *message)
{
    // Run all functions in the event structure

    for (size_t i = 0; i < oyamo_event->length; i++)
    {
        // If the function is not NULL, call it
        if (oyamo_event->functions[i])
        {
            oyamo_event->functions[i](client, message);
        }
    }
}

void oyamo_events_free(oyamo_events_struct_T *events)
{
    // Free the 'onconnect' event's functions array
    free(events->onconnect->functions);

    // Free the 'onconnect' event structure
    free(events->onconnect);

    // Free the 'ondisconnect' event's functions array
    free(events->ondisconnect->functions);

    // Free the 'ondisconnect' event structure
    free(events->ondisconnect);

    // Free the 'onmessage' event's functions array
    free(events->onmessage->functions);

    // Free the 'onmessage' event structure
    free(events->onmessage);

    // Free the 'oyamo_events_struct_T' structure
    free(events);
}