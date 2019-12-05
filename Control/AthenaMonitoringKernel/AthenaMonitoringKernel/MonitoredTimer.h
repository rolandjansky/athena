/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_MonitoredTimer_h
#define AthenaMonitoringKernel_MonitoredTimer_h

#include <chrono>
#include <vector>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"

namespace Monitored {
  void checkNamingConvention( const std::string& name );
  /**
   * Monitored Timer
   *
   * The time is measured either between explicit stop/start calls or between the creation
   * and the time the value is read by the monitoring tool.
   *
   * The template parameter defines the unit of elapsed time measurement. 
   * See for all options: https://en.cppreference.com/w/cpp/chrono/duration
   *
   * The timer name needs to start with the string "TIME_".
   * \code
   *    auto t1 = Monitored::Timer("TIME_t1");
   * \endcode
   **/
  template< typename unit=std::chrono::microseconds>
  class Timer : public IMonitoredVariable {
  public:
    Timer(std::string name );
    Timer(Timer&&) = default;
    Timer(Timer const&) = delete;

    void start();      //<! (re)starts the timer
    void stop();       //<! stops the timer

    operator double() const; //!< duration between start and stop (or current time) in microseconds

    const std::vector<double> getVectorRepresentation() const override { return {double(*this)}; }

    virtual std::vector<std::string> getStringVectorRepresentation() const override { return std::vector<std::string>(); };
    virtual bool hasStringRepresentation() const override { return false; };

  private:

    typedef std::chrono::high_resolution_clock clock_type;
    clock_type::time_point m_startTime;
    clock_type::time_point m_stopTime;

    Timer& operator=(Timer const&) = delete;
  };


  template<typename unit>
  Timer<unit>::Timer(std::string tname) : IMonitoredVariable(std::move(tname)) {
    checkNamingConvention( name() );
    start();
  }
  template<typename unit>
  void Timer<unit>::start() { m_startTime = clock_type::now(); }

  template<typename unit>
  void Timer<unit>::stop() { m_stopTime = clock_type::now(); }

  template<typename unit>
  Timer<unit>::operator double() const {
    clock_type::time_point stopTime = m_stopTime;
    if (stopTime == clock_type::time_point()) // never stopped
      stopTime = clock_type::now();
    auto d = std::chrono::duration_cast<unit>(stopTime - m_startTime);
    return d.count();
  }

} // namespace Monitored

#endif /* AthenaMonitoringKernel_MonitoredTimer_h */
