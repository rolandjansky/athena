/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HltExceptions_H
#define HltExceptions_H

#include <exception>
#include <string_view>
#include <string>

namespace hltonl::Exception {
  /**
   * @class NoMoreEvents
   * @brief Thrown if all events are already read from the input and another one is requested
   */
  class NoMoreEvents : public std::exception {
    public:
      const char* what() const noexcept override {return "No more events to be processed";}
  };
  /**
   * @class NoEventsTemporarily
   * @brief Thrown if the event source cannot provide new events temporarily, e.g. when trigger is on hold
   */
  class NoEventsTemporarily : public std::exception {
    public:
      const char* what() const noexcept override {return "No events can be provided temporarily";}
  };
  /**
   * @class EventSourceCorrupted
   * @brief Thrown if event source throws an exception when new event is requested
   */
  class EventSourceCorrupted : public std::exception {
    public:
      const char* what() const noexcept override {return "Event source corrupted and cannot provide events";}
  };
  /**
   * @class MissingCTPFragment
   * @brief Thrown if the CTP ROBFragment cannot be retrieved for a new event
   */
  class MissingCTPFragment : public std::exception {
    public:
      const char* what() const noexcept override {return "Missing CTP ROBFragment";}
  };
  /**
   * @class BadCTPFragment
   * @brief Thrown if the CTP ROBFragment for a new event has non-zero status word or other errors
   */
  class BadCTPFragment : public std::exception {
    public:
      BadCTPFragment(std::string_view reason) {
        m_reason.append(reason);
      }
      const char* what() const noexcept override {return m_reason.data();}
    private:
      std::string m_reason{"Bad CTP ROBFragment: "};
  };
}

#endif // HltExceptions_H
