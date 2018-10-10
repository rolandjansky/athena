// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorSliceCPU_H
#define TrigCostRootAnalysis_MonitorSliceCPU_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "MonitorBase.h"

namespace TrigCostRootAnalysis {
  //Forward declaration
  class TrigCostData;

  /**
   * @class MonitorSliceCPU
   * Keep track of CPU usage per chain
   */
  class MonitorSliceCPU: public MonitorBase {
  public:
    MonitorSliceCPU(const TrigCostData* costData);
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();
  }; //class MonitorSliceCPU
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorSliceCPU_H
