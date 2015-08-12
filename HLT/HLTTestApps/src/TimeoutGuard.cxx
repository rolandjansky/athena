/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TimeoutGuard.cxx 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Implementation of our timeout watchdog. 
 */

#include "TimeoutGuard.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/format.hpp>
#include "ers/ers.h"
#include "issue.h"

class BooleanPredicate {
  private:
    const bool& m_value;
  public:
    BooleanPredicate(const bool& value): m_value(value) {}
    bool operator()() const { return m_value; }
};

/**
 * Private helper
 */
struct HLTTestApps::TimeoutGuard::Watchdog {

  void operator()(unsigned int timeout_ms, 
                  float warn_fraction, 
                  HLTTestApps::TimeoutGuard& guard,
                  hltinterface::HLTInterface* processor) {

    const static boost::property_tree::ptree empty_ptree;
    unsigned int warn_time_ms = unsigned(timeout_ms * warn_fraction);
    unsigned int additional_error_time_ms = timeout_ms - warn_time_ms;
    BooleanPredicate start_pred(guard.m_can_start);
    BooleanPredicate reset_pred(guard.m_reset);

    while (!guard.m_stop) {
      
      boost::unique_lock<boost::mutex> lock(guard.m_mutex);
      //temporarily unlocks mutex, waits for start signal
      guard.m_condition.wait(lock, start_pred);
      guard.m_can_start = false;

      //temporarily unlocks mutex, waits for reset signal or timeout
      if (guard.m_condition.timed_wait(lock, boost::posix_time::milliseconds(warn_time_ms), reset_pred)) {
        guard.m_reset = false;
        continue;
      }

      processor->timeOutReached(empty_ptree);
      boost::format msg1("Reached \"soft timeout\" - %f%% (%d ms) of the total timeout value (%d ms). Asking HLT to wrap-up event processing...");
      msg1 % (100.0*warn_time_ms/timeout_ms) % warn_time_ms % timeout_ms;
      ers::warning(HLTTESTAPPS_UNCLASSIFIED(msg1.str()));

      //temporarily unlocks mutex, waits for reset signal or timeout
      if (guard.m_condition.timed_wait(lock, boost::posix_time::milliseconds(additional_error_time_ms), reset_pred)) {
        guard.m_reset = false;
        continue;
      }
      
      boost::format msg2("Reached the \"hard timeout\" value (%d ms)");
      msg2 % timeout_ms;
      ers::error(HLTTESTAPPS_UNCLASSIFIED(msg2.str()));

      // Got hard timeout, need to wait for reset before continuing
      guard.m_condition.wait(lock, reset_pred);
      guard.m_reset = false;
    }

  }
};

HLTTestApps::TimeoutGuard::TimeoutGuard(unsigned int timeout_ms,
    float warn_fraction, hltinterface::HLTInterface* processor)
  : m_stop(false),
    m_mutex(),
    m_condition(),
    m_can_start(false),
    m_reset(false),
    m_timer(0)
{
  if (timeout_ms != 0) 
    m_timer = new boost::thread(Watchdog(), timeout_ms, warn_fraction, 
        boost::ref(*this), processor);
}

HLTTestApps::TimeoutGuard::~TimeoutGuard() 
{
  if (m_timer) {
    m_stop = true;

    // try to reach one of the stop checks 
    start();
    reset();

    m_timer->join();
    delete m_timer;
    m_timer = 0;
  }
}

void HLTTestApps::TimeoutGuard::start()
{
  if (m_timer) {
    boost::unique_lock<boost::mutex> lock(m_mutex);
    m_can_start = true;
    m_condition.notify_one();
  }
}

void HLTTestApps::TimeoutGuard::reset()
{
  if (m_timer) {
    boost::unique_lock<boost::mutex> lock(m_mutex);
    m_reset = true;
    m_condition.notify_one();
  }
}
