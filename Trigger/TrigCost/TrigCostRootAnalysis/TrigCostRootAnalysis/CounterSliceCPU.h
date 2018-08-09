// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterSliceCPU_H
#define TrigCostRootAnalysis_CounterSliceCPU_H

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
   * @class CounterSliceCPU
   * @see CounterBase
   */
  class CounterSliceCPU: public CounterBase {
  public:
    CounterSliceCPU(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
                    MonitorBase* parent = 0);
    ~CounterSliceCPU();
    void startEvent();
    void processEventCounter(UInt_t e, UInt_t f, Float_t weight = 1.);
    void endEvent(Float_t weight = 1);
  private:
    Double_t getPrescaleFactor(UInt_t e = INT_MAX);
  }; //class CounterSliceCPU
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterSliceCPU_H
