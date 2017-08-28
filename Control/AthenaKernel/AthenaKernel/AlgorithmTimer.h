/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ALGORITHMTIMER_H
#define ATHENAKERNEL_ALGORITHMTIMER_H

/**
 * @file   AthenaKernel/AlgorithmTimer.h
 * @brief  Efficient realtime timers
 * @author Rolf Seuster, Frank Winklmeier
 *     (based on TrigTimeAlgs/CookTimer, which in turn is based in AthenaKernel/CookTimer by W. Lavrijsen, S. Binet)
 *
 * $Id: AlgorithmTimer.h,v 1.3 2009-05-14 16:58:21 dquarrie Exp $
 */

// Framework includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// System includes
#include <time.h>
#include <signal.h>
#ifdef __APPLE__
#include <sys/signal.h>
#include <sys/time.h>
#endif

// STL includes
#include <string>
#include <list>

// Boost includes
#include <boost/function.hpp>

#ifdef __APPLE__
typedef sigval sigval_t;
typedef itimerval timer_t;
#endif //MacOSX

namespace Athena {

  //forward declarations
  class ScopedTimer;
  
  namespace AlgorithmTimerHandler
  {
    void onAlarmThread(sigval_t sv);
  }

  /**
   * @class AlgorithmTimer
   * @brief Timer that invokes a user callback once the time is reached
   *
   * AlgorithmTimer automatically disables itself in its destructor. Create it
   * on the stack and it will therefore disable itself once it runs out of
   * scope. Example:
   *  
   * \code
   * StatusCode MyAlgorithm::execute()
   * {
   *   Athena::AlgorithmTimer(2000, myAlarmHandler);
   *   // something that takes a long time
   * }
   * \endcode
   *
   * The overhead of creating a AlgorithmTimer instance is very small. If you are
   * still worried about it, you can re-use the instance by creating it outside
   * the scope and starting/stopping it manually via its start() and stop() method.
   * 
   * To get the best of both worlds you can create the AlgorithmTimer outside the
   * scope and use an even smaller ScopedTimer instance to start the timer. Once the
   * ScopedTimer instance runs out of scope it will disarm the associated AlgorithmTimer.
   *
   * \code
   * MyAlgorithm::MyAlgorithm() :
   *   m_algorithmTimer(0, myAlarmHandler)   // create it in disabled state
   * {     
   * }
   *
   * StatusCode MyAlgorithm::execute()
   * {
   *   Athena::ScopedTimer alarm(2000);  // two seconds
   *   m_algorithmTimer.start(alarm);    // this will start the timer
   *   // something that takes a long time
   * }
   * \endcode
   *
   */  
  class AlgorithmTimer
  {
    friend void AlgorithmTimerHandler::onAlarmThread(sigval_t sv);
  
  public:

    /**
     * @brief Configuration flags for AlgorithmTimer
     */
    enum AlgorithmTimerConfig
      {
        DEFAULT          = 0x0,    ///< default
        USEREALTIME      = 0x1,    ///< use real time instead of system time
        DELIVERYBYTHREAD = 0x2     ///< deprecated; delivery is always by thread.
      };
  
    typedef boost::function<void()> callbackFct_t;
  
    /**
     * @brief Create AlgorithmTimer instance and start it (unless seconds==0)
     * @param milliseconds     Time in milliseconds until expiration of the timer (0 = disabled)
     * @param callback         User callback to invoke once time is reached
     * @param useRealTime      true, if timer should use ral instead od CPU time
     */
    AlgorithmTimer(unsigned int milliseconds,
                   callbackFct_t callback = NULL,
                   AlgorithmTimerConfig conf = DEFAULT);
  
    /**
     * @brief Destroy and disable the timer
     */
    ~AlgorithmTimer();
  
    /**
     * @brief (Re)start the timer with the last timeout used
     */
    void start() { start(m_timeout); }
  
    /**
     * @brief Start the timer
     * @param milliseconds  Time in milliseconds until expiration
     */
    void start(unsigned int milliseconds);

    /**
     * @brief Start the timer
     * @param scope  ScopedTimer instance to automatically stop the timer
     */
    void start(ScopedTimer& scope);
  
    /**
     * @brief Stop the timer and return the time left in [ms].
     */
    unsigned int stop();
  
    /**
     * @brief Returns the time left in milliseconds
     */
    unsigned int timeLeft() const;
  
    /**
     * @brief Returns the currently set timeout (in milliseconds)
     */
    unsigned int timeout() const { return m_timeout; }
  
    /**
     * @brief set the levelof details for (optional) gdb core dump
     */
    void SetGDBCoreDumpDetails(unsigned int details)
    { m_gdb_details = details; };
  
  protected:
  
    unsigned int     m_timeout;         ///< timeout in milliseconds
    struct sigevent  m_sigevent;        ///< for signal handler
    timer_t          m_timerid;         ///< timer ID
    callbackFct_t    m_onAlarm;         ///< user callback 
  
  
  private:
    // Dummies
    AlgorithmTimer();                                 ///< no default constructor
    AlgorithmTimer(const AlgorithmTimer&);            ///< no copying allowed
    AlgorithmTimer& operator=(const AlgorithmTimer&); ///< no copying allowed
  
    callbackFct_t abortJob();
  
    unsigned int m_gdb_details;
  };


  /**
   * @class ScopedTimer
   * @brief Helper class to create a "scoped cook timer" without having to declare
   *        the CookTimer itself within the scope (see CookTimer documentation)
   */
  class ScopedTimer {
    friend class AlgorithmTimer;
  public:
    ScopedTimer(unsigned int milliseconds) : m_timeout(milliseconds), m_timer(0) {}
    ~ScopedTimer() { if (m_timer) m_timer->stop(); }
  private:
    unsigned int m_timeout;        ///< Timeout in milliseconds
    AlgorithmTimer* m_timer;       ///< AlgorithmTimer instance, set by AlgorithmTimer::start()
  };
  
} // namespace Athena

#endif
