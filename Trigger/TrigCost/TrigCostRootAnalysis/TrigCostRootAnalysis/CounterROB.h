// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterROB_H
#define TrigCostRootAnalysis_CounterROB_H

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
   * @class CounterROB
   * Counter to monitor the data fetch requests and data cache requests for a single Read Out Buffer, or RO.
   * @see CounterBase
   */
  class CounterROB: public CounterBase {
  public:
    CounterROB(const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10,
               MonitorBase* _parent = 0);
    ~CounterROB();
    void startEvent();
    void processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight = 1.);
    void endEvent(Float_t _weight = 1.);
    void debug(UInt_t _e, UInt_t _f);
  private:
    Double_t getPrescaleFactor(UInt_t _e = INT_MAX);
  }; //class CounterROB
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterROB_H
