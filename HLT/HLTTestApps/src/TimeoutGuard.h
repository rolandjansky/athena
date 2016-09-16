/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TimeoutGuard.h 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Implements a python'able interface to a timeout thread. 
 */

#ifndef HLTTESTAPPS_TIMEOUTGUARD_H 
#define HLTTESTAPPS_TIMEOUTGUARD_H

#include "hltinterface/HLTInterface.h"
#include <boost/thread.hpp>

namespace HLTTestApps {
  
  /**
   * A threaded timer that keeps track of the time the HLT takes to process an
   * event, notifying it when a timeout is imminent and issuing (logging, no
   * exception is thrown) an error when the timeout is reached. The timer thread
   * is started at the first call of start() (not upon object construction). It
   * is stopped either when the instance is destroyed or when the timeout is
   * reached (whichever happens first). A TimeoutGuard can also be reset() and
   * re- start() -ed. This allows reusing the same instance -- typically for all
   * the events in a process loop - as well as the timer thread itself --
   * which is not stopped or recreated, only held and released.
   */
  class TimeoutGuard {
  public:

    /**
     * Constructor.
     *
     * @param timeout_ms The total timeout to be used for indicating an event is
     * processing more than what was expected. In milliseconds.
     * @param warn_fraction A fraction of the total time that should be used to
     * warn the HLT framework a timeout is imminent. For instance, a
     * warn_fraction of 0.8 means the HLT is warned about an imminent timeout
     * when 80% of the time has elapsed.
     * @param processor A pointer to the HLT implementation currently
     * processing the event.
     */
    TimeoutGuard(unsigned int timeout_ms, 
                 float warn_fraction, 
                 hltinterface::HLTInterface* processor);

    /**
     * D'tor. Stops and joins the timer thread it it is running.
     */
    virtual ~TimeoutGuard();

    /**
     * Launches the timer thread to start accounting for the current event being
     * processed.
     */
    void start();

    /**
     * Reset and stop the current counting until the next start. Notice the
     * timer thread is kept alive and ready to go again until either start or
     * the d'tor is called.
     */
    void reset();

  private:
    struct Watchdog;

    bool m_stop;
    boost::mutex m_mutex;
    boost::condition_variable m_condition;
    bool m_can_start;
    bool m_reset;
    boost::thread* m_timer;
  };

}

#endif /* HLTTESTAPPS_TIMEOUTGUARD_H */

