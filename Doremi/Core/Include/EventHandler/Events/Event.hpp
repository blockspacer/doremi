#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
            Every new type of event needs to be inserted in this enum
        */
        enum class EventType
        {
            Example, // Only an example
            PlayerCreation,
        };
        /**All events needs to inherit this struct*/
        struct Event
        {
            Event(EventType p_eventType) : eventType(p_eventType) {}
            // Since all events inherits this struct we can store the event type here
            EventType eventType;
        };
    }
}
