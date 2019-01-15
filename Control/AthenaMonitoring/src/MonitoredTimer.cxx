/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/algorithm/string/predicate.hpp>
#include <ratio>

#include "AthenaMonitoring/MonitoredTimer.h"
#include <iostream>

namespace Monitored {

  constexpr const char* Timer::TIMER_PREFIX;

  Timer::Timer(std::string name) : IMonitoredVariable(std::move(name)) {
    // Enforce some naming convention for timers
    if (!boost::algorithm::starts_with(m_name, TIMER_PREFIX)) {
      throw std::runtime_error("Name of Timer \"" + m_name + "\" needs to start with \"" +
                               TIMER_PREFIX + "\"");
    }
    start();
  }

  void Timer::start() { m_startTime = clock_type::now(); }

  void Timer::stop() const { m_stopTime = clock_type::now(); }

  Timer::operator double() const {
    if (m_stopTime == clock_type::time_point()) // never stoppped
      stop();
    auto d = std::chrono::duration_cast<std::chrono::microseconds>(m_stopTime - m_startTime);
    return d.count();
  }

} // namespace Monitored
