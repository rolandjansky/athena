// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorAlgorithmClass_H
#define TrigCostRootAnalysis_MonitorAlgorithmClass_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "MonitorBase.h"
#include "MonitorAlgorithmCommon.h"

namespace TrigCostRootAnalysis {
  //Forward declaration
  class TrigCostData;

  /**
   * @class MonitorAlgorithmClass
   * Base algorithm class monitor implimentation.
   */
  class MonitorAlgorithmClass: public MonitorBase, public MonitorAlgorithmCommon {
  public:
    MonitorAlgorithmClass(const TrigCostData* costData);
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();
  }; //class MonitorAlgorithmClass
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorAlgorithmClass_H
