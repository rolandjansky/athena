// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterChain_H
#define TrigCostRootAnalysis_CounterChain_H

// STL include(s):
#include <map>
#include <string>

// Local include(s):
#include "CounterBase.h"

// ROOT include(s):
#include <Rtypes.h>

namespace TrigCostRootAnalysis {

  //Forward declaration
  class TrigCostData;
  
  /**
   * @class CounterChain
   * One counter chain is created per monitored chain in the run. It is responsible for
   * recording the monitoring information for that chain in every event in which it
   * is run.
   * @see CounterBase
   */
  class CounterChain : public CounterBase {
  
   public:
   
    CounterChain( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10 );
    ~CounterChain();
    void startEvent();
    void processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight = 1.);
    void endEvent();
    void debug(UInt_t _e);
    
   private:
   
    static Float_t s_eventTimeExecute; //<! Global tabulator of total chain time for all chains in an event
    
  }; //class CounterChain
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterChain_H
