/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HltExceptions_H
#define HltExceptions_H

#include <exception>

namespace hltonl {
  namespace Exception {
    /**
     * @class NoMoreEvents
     * @brief Thrown if all events are already read from the input and another one is requested
     */
    class NoMoreEvents : public std::exception {
      public:
        NoMoreEvents() {}
        virtual ~NoMoreEvents() {}
        const char* what() const noexcept override {return "No more events to be processed";}
    };
    /**
     * @class NoEventsTemporarily
     * @brief Thrown if the event source cannot provide new events temporarily, e.g. when trigger is on hold
     */
    class NoEventsTemporarily : public std::exception {
      public:
        NoEventsTemporarily() {}
        virtual ~NoEventsTemporarily() {}
        const char* what() const noexcept override {return "No events can be provided temporarily";}
    };
    /**
     * @class EventSourceCorrupted
     * @brief Thrown if event source keeps throwing exceptions when new event is requested
     */
    class EventSourceCorrupted : public std::exception {
      public:
        EventSourceCorrupted() {}
        virtual ~EventSourceCorrupted() {}
        const char* what() const noexcept override {return "Event source corrupted and cannot provide events";}
    };
  }
}

#endif // HltExceptions_H
