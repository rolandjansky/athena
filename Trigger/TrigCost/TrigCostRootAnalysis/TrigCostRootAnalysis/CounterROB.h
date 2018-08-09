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
    CounterROB(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
               MonitorBase* parent = 0);
    ~CounterROB();
    void startEvent();
    void processEventCounter(UInt_t e, UInt_t f, Float_t weight = 1.);
    void endEvent(Float_t weight = 1.);
    void debug(UInt_t e, UInt_t f);
  private:
    Double_t getPrescaleFactor(UInt_t e = INT_MAX);
  }; //class CounterROB
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterROB_H
