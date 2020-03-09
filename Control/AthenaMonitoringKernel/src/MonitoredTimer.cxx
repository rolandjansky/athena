/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include <boost/algorithm/string/predicate.hpp>
#include "AthenaMonitoringKernel/MonitoredTimer.h"

namespace Monitored {
  void checkNamingConvention( const std::string& name ) {
    // Enforce some naming convention for timers
    const char* TIMER_PREFIX = "TIME_"; //<! prefix required for all timers
    if (!boost::algorithm::starts_with(name, TIMER_PREFIX)) {
      throw std::runtime_error("Name of Timer \"" + name + "\" needs to start with \"" +
                               TIMER_PREFIX + "\"");
    }

  }

} // namespace Monitored
