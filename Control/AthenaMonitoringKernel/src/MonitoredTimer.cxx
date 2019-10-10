/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/algorithm/string/predicate.hpp>

#include "AthenaMonitoringKernel/MonitoredTimer.h"

namespace Monitored {

  constexpr const char* Timer::TIMER_PREFIX;

  Timer::Timer(std::string tname) : IMonitoredVariable(std::move(tname)) {
    // Enforce some naming convention for timers
    if (!boost::algorithm::starts_with(name(), TIMER_PREFIX)) {
      throw std::runtime_error("Name of Timer \"" + name() + "\" needs to start with \"" +
                               TIMER_PREFIX + "\"");
    }
    start();
  }

  void Timer::start() { m_startTime = clock_type::now(); }

  void Timer::stop() { m_stopTime = clock_type::now(); }

  Timer::operator double() const {
    clock_type::time_point stopTime = m_stopTime;
    if (stopTime == clock_type::time_point()) // never stopped
      stopTime = clock_type::now();
    auto d = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - m_startTime);
    return d.count();
  }

} // namespace Monitored
