// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorFullEvent_H
#define TrigCostRootAnalysis_MonitorFullEvent_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "MonitorBase.h"

// ROOT includes
#include <TRandom3.h>

namespace TrigCostRootAnalysis {

  //Forward declaration
  class Data;
  
  /**
   * @class MonitorFullEvent
   * Per event mapping of the trigger element hieararchy.
   */
  class MonitorFullEvent : public MonitorBase {
  
   public:
   
    MonitorFullEvent(const TrigCostData* _costData);
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter( const std::string &_name, Int_t _ID );
    void saveOutput();

   private:

    Float_t  m_probability; //!< Probability that an event will be saved.
    TRandom3 m_rando; //!< Random number generator used to decide if the event is to be saved.
    
  }; //class MonitorFullEvent
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorFullEvent_H
