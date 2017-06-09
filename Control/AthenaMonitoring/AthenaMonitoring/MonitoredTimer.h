/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredTimer_h
#define AthenaMonitoring_MonitoredTimer_h

#include <vector>
#include <chrono>

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/MonitoredHelpers.h"

namespace Monitored {
    namespace MonitoredTimer {

        class MonitoredTimer;       
        MonitoredTimer declare(std::string name);

	/**
	 * @class Timer class suitable for insertions to monitoring histograms
	 * The time is measured from execution of the start to stop 
	 * And if they are not used
	 * from creation to the moment they are harvested to monitoring output (call of conversion to double operator)
	 **/
	
        class MonitoredTimer : public IMonitoredVariable {
        public:
	  
            friend MonitoredTimer declare(std::string name);
            
            MonitoredTimer(MonitoredTimer&&) = default;

	    /**
	     * @brief (re)starts the timer 
	     **/
	    void start();
	    /**
	     * @brief stops the timer
	     **/
	    void stop() const;

	    /**
	     * @brief returns duration between the start and stop in microseconds
	     **/	    
            operator double() const;
            
            const std::vector<double> getVectorRepresentation() const override { return { double(*this) }; }
        private:
	    typedef std::chrono::high_resolution_clock clock_type;
	    clock_type::time_point m_startTime; 
	    mutable clock_type::time_point m_stopTime;  
	    
            MonitoredTimer(std::string name);
            MonitoredTimer(MonitoredTimer const&) = delete;
            MonitoredTimer& operator=(MonitoredTimer const&) = delete;
        };
        

        MonitoredTimer declare(std::string name) {
            return MonitoredTimer(std::move(name));
        }
    }
}

#endif /* AthenaMonitoring_MonitoredTimer_h */
