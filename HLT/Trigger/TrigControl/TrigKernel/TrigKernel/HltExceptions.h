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
     * @brief Can be thrown if all events are already read from the input and another one is requested
     */
    class NoMoreEvents : public std::exception {
      public:
        NoMoreEvents() {}
        virtual ~NoMoreEvents() {}
        const char* what() const noexcept override {return "No more events to be processed";}
    };
  }
}

#endif // HltExceptions_H
