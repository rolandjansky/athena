/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ratio>
#include <boost/algorithm/string/predicate.hpp>

#include "AthenaMonitoring/MonitoredTimer.h"
#include <iostream>

namespace Monitored {
  namespace MonitoredTimer{

    constexpr const char* MonitoredTimer::TIMER_PREFIX;

    MonitoredTimer::MonitoredTimer(std::string name)
    : IMonitoredVariable(std::move(name)) {
      // Enforce some naming convention for timers
      if (!boost::algorithm::starts_with(m_name, TIMER_PREFIX)) {
        throw std::runtime_error("Name of MonitoredTimer \""+m_name+"\" needs to start with \""+TIMER_PREFIX+"\"");
      }
      start();
    }
    
    void MonitoredTimer::start() {
      m_startTime = clock_type::now();
    }

    void MonitoredTimer::stop() const {
      m_stopTime = clock_type::now();
    }

    MonitoredTimer::operator double() const {
      if ( m_stopTime == clock_type::time_point() )  // never stoppped
	stop();
      auto d =  std::chrono::duration_cast<std::chrono::microseconds>(m_stopTime - m_startTime);
      return d.count();
    }
  }
}
