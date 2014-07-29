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
  
  struct TimeoutGuard {

    /**
     * Constructor.
     *
     * @param timeout_ms The total timeout to be used for indicating an event is
     * processing more than what was expected. In milliseconds.
     * @param warn_fraction A fraction of the total time that should be used to
     * warn the HLT framework a timeout is imminent.
     * @param processor A pointer to the HLT implementation currently
     * processing the event.
     */
    TimeoutGuard(); //does nothing in this case
    TimeoutGuard(unsigned int timeout_ms, 
                 float warn_fraction, 
                 hltinterface::HLTInterface* processor);

    /**
     * Dest.
     */
    virtual ~TimeoutGuard();

    /**
     * Start accounting for the current event being processed.
     */
    void start();

    /**
     * Stop the current counter and wait for another start()
     */
    void reset();

    bool m_stop;
    boost::mutex m_mutex;
    boost::condition_variable m_condition;
    bool m_can_start;
    bool m_reset;
    boost::thread* m_timer;
  };

}

#endif /* HLTTESTAPPS_TIMEOUTGUARD_H */

