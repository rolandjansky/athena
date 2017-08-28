/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_TIMEOUT_H
#define ATHENAKERNEL_TIMEOUT_H 1

/**
 * @file   Timeout.h
 * @brief  Timeout singleton
 * @author Frank Winklmeier
 *
 * $Id: Timeout.h,v 1.2 2008-12-09 16:48:42 fwinkl Exp $
 */


#include "AthenaKernel/SlotSpecificObj.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <atomic>


namespace Athena {
  
  // Forward declarations
  class TimeoutMaster;

  /**
   * @class Timeout
   * @brief Singleton to access the timeout flag
   *
   * This singleton provides a global timeout flag. It can be set
   * by the TimeoutMaster and algorithms, tools, etc. can query it
   * in loops to react on the timeout signal.
   */
  class Timeout {
    /// Only classes derived from TimeoutMaster can modify timeout flag
    friend class TimeoutMaster;
  
  public:
    /// Get reference to Timeout singleton
    static Timeout& instance();
    static Timeout& instance(const EventContext& ctx);

    /// Check if the timeout was reached
    bool reached() const { return m_state; }
  
    Timeout() : m_state(false) {}

  private:
    std::atomic<bool> m_state;          ///< Timeout flag
    void set()   { m_state = true; }    ///< Set timeout flag
    void reset() { m_state = false; }   ///< Reset timeout flag
  
    // Prevent direct instantiation 
    Timeout(Timeout&);
    Timeout& operator=(const Timeout&);
  };

  inline Timeout& Timeout::instance (const EventContext& ctx) {
    static SG::SlotSpecificObj<Timeout> instances ATLAS_THREAD_SAFE;
    return *instances.get (ctx);
  }

  inline Timeout& Timeout::instance() {
    return instance (Gaudi::Hive::currentContext());
  }

  
  /**
   * @class TimeoutMaster
   * @brief Class to modify timeout flag
   *
   * Only classes inheriting from TimeoutMaster are allowed
   * to modify the Timeout singleton. In practice, this should be
   * the responsibility of the EventLoopMgr.
   */
  class TimeoutMaster {
  protected:
    /// Set timeout
    void setTimeout(Timeout& instance) { instance.set(); }

    /// Reset timeout
    void resetTimeout(Timeout& instance) { instance.reset(); }

    /// Prevent direct instantiation
    TimeoutMaster() {}
    TimeoutMaster(TimeoutMaster&);
    TimeoutMaster& operator=(const TimeoutMaster&);
  };

} // namespace Athena

#endif
