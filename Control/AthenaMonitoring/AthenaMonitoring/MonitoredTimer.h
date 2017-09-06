/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredTimer_h
#define AthenaMonitoring_MonitoredTimer_h

#include <vector>
#include <chrono>

#include "AthenaMonitoring/IMonitoredVariable.h"

namespace Monitored {
  namespace MonitoredTimer {

    class MonitoredTimer;

    /**
     * Declare a monitored timer
     *
     * The timer name needs to start with "TIME_"
     *
     * \code
     *    auto t1 = MonitoredTimer::declare("TIME_t1");
     * \endcode
     **/
    MonitoredTimer declare(std::string name);

	/**
	 * Monitored Timer
     *
     * The time is measured either between explicit stop/start calls or between the creation
     * and the time the value is read by the monitoring tool.
     * The timer name needs to start with the string "TIME_".
	 **/	
    class MonitoredTimer : public IMonitoredVariable {
    public:
	  
      friend MonitoredTimer declare(std::string name) {
        return MonitoredTimer(std::move(name));
      }
      
      MonitoredTimer(MonitoredTimer&&) = default;

      void start();         //<! (re)starts the timer
      void stop() const;    //<! stops the timer

      operator double() const; //!< duration between start and stop in microseconds
            
      const std::vector<double> getVectorRepresentation() const override { return { double(*this) }; }

    private:
      static constexpr const char* TIMER_PREFIX = "TIME_";   //<! prefix required for all timers

      typedef std::chrono::high_resolution_clock clock_type;
      clock_type::time_point m_startTime; 
      mutable clock_type::time_point m_stopTime;  
	    
      MonitoredTimer(std::string name);
      MonitoredTimer(MonitoredTimer const&) = delete;
      MonitoredTimer& operator=(MonitoredTimer const&) = delete;
    };        
  }
}

#endif /* AthenaMonitoring_MonitoredTimer_h */
