// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <time.h>

// Local include(s):
#include "../TrigCostRootAnalysis/Timer.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT include(s):
#include <TError.h>

namespace TrigCostRootAnalysis {
  std::vector<Timer*> Timer::s_timers;

  /**
   * Base class constructor. Sets name and ID.
   */
  Timer::Timer(std::string type, std::string name) : m_type(type), m_name(name), m_startTime(0), m_time(0), m_calls(
      0) {
    s_timers.push_back(this);
  }

  Timer::~Timer() {
  }

  void Timer::start() {
    m_startTime = clock();
  }

  void Timer::stop() {
    m_time += ((Float_t) clock() - m_startTime) / (Float_t) CLOCKS_PER_SEC * 1000.;
    ++m_calls;
  }

  void Timer::print() {
    if (m_calls == 0) return;

    Float_t avTime = m_time / m_calls;
    Info("Timer::print", "%s timer used\t %.3f ms per call over %i call(s) in '%s'",
         m_type.c_str(), avTime, m_calls, m_name.c_str());
  }

  void Timer::printAll() {
    for (UInt_t t = 0; t < s_timers.size(); ++t) {
      s_timers.at(t)->print();
    }
  }
} // namespace TrigCostRootAnalysis
