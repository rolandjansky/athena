#include <ratio>
#include "AthenaMonitoring/MonitoredTimer.h"

namespace Monitored {
  namespace MonitoredTimer{
    MonitoredTimer::MonitoredTimer(std::string name)
    : IMonitoredVariable(std::move(name)) {
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
