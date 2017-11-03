// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_Timer_H
#define TrigCostRootAnalysis_Timer_H

// STL include(s):
#include <string>
#include <vector>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "Utility.h"

/**
 * @file TrigCostRootAnalysis/Timer.h
 *
 * A simple class to keep track of time useage
 *
 */

namespace TrigCostRootAnalysis {
  /**
   * @class Timer
   * Has a name, and does average time
   */
  class Timer {
  public:
    Timer(std::string _type, std::string _name);
    ~Timer();
    void start();
    void stop();
    void print();

    static void printAll();
  private:
    std::string m_type; //<! What type of thing are we timing
    std::string m_name; //!< Name of what we are doing the time for
    Float_t m_startTime; //<! Holds when the timer is started
    Float_t m_time; //<! Holds the total elapsed time
    UInt_t m_calls; //!< Holds the number of calls
    static std::vector<Timer*> s_timers; //!< Holds a pointer to every timer which has been minted
  };  //class Timer
}

#endif //TrigCostRootAnalysis_Timer_H
