/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_MonitoredTimer_h
#define AthenaMonitoringKernel_MonitoredTimer_h

#include <chrono>
#include <vector>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"

namespace Monitored {

  /**
   * Monitored Timer
   *
   * The time is measured either between explicit stop/start calls or between the creation
   * and the time the value is read by the monitoring tool.
   *
   * The timer name needs to start with the string "TIME_".
   * \code
   *    auto t1 = Monitored::Timer("TIME_t1");
   * \endcode
   **/
  class Timer : public IMonitoredVariable {
  public:
    Timer(std::string name);
    Timer(Timer&&) = default;
    Timer(Timer const&) = delete;

    void start();      //<! (re)starts the timer
    void stop();       //<! stops the timer

    operator double() const; //!< duration between start and stop (or current time) in microseconds

    const std::vector<double> getVectorRepresentation() const override { return {double(*this)}; }

    virtual std::vector<std::string> getStringVectorRepresentation() const override { return std::vector<std::string>(); };
    virtual bool hasStringRepresentation() const override { return false; };

  private:
    static constexpr const char* TIMER_PREFIX = "TIME_"; //<! prefix required for all timers

    typedef std::chrono::high_resolution_clock clock_type;
    clock_type::time_point m_startTime;
    clock_type::time_point m_stopTime;

    Timer& operator=(Timer const&) = delete;
  };
} // namespace Monitored

#endif /* AthenaMonitoringKernel_MonitoredTimer_h */
