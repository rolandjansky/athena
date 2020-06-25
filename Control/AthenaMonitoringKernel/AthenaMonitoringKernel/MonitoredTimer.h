/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_MonitoredTimer_h
#define AthenaMonitoringKernel_MonitoredTimer_h

#include <chrono>
#include <vector>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"

namespace Monitored {
  void checkNamingConvention( const std::string& name );
  /**
   * A monitored timer.
   *
   * The time is measured either between explicit stop/start calls or between the creation
   * and the time the value is read by the monitoring tool.
   *
   * A strict naming convention is enforced. <b>Timers need to start with the string "TIME_".</b>
   *
   * \tparam unit  Unit of elapsed time (std::chrono::duration)
   *
   * #### Examples
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx timerFilling
   *
   * @see Monitored::ScopedTimer
   */
  template< typename unit=std::chrono::microseconds>
  class Timer : public IMonitoredVariable {
  public:
    Timer(std::string name );
    Timer(Timer&&) = default;
    Timer(Timer const&) = delete;

    void start();      //<! (re)starts the timer
    void stop();       //<! stops the timer

    operator double() const; //!< duration (in unit) between start and stop (or current time)

    virtual double get(size_t) const override { return double(*this); }
    virtual std::string getString([[maybe_unused]] size_t i) const override { return {}; }
    virtual bool hasStringRepresentation() const override { return false; };
    virtual size_t size() const override { return 1; }
    
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

  /**
   * Helper class to create a scoped timer.
   *
   * This helper will call start()/stop() of the specified Monitored::Timer
   * on creation/destruction. It is useful in case there is unrelated code
   * between creation of the Monitored::Timer and the code of interest.
   *
   * \code
   *    auto t1 = Monitored::Timer("TIME_t1");
   *    // unrelated code
   *    {
   *       ScopedTimer timeit(t1);
   *       // code to be timed
   *    }
   * \endcode
   */
  template<typename T>
  class ScopedTimer {
  public:
    ScopedTimer(T& timer) : m_timer(timer) {
      m_timer.start();
    }
    ~ScopedTimer() {
      m_timer.stop();
    }
  private:
    T& m_timer;
  };

} // namespace Monitored

#endif /* AthenaMonitoringKernel_MonitoredTimer_h */
