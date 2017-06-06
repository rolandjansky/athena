// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorAlgorithmChain_H
#define TrigCostRootAnalysis_MonitorAlgorithmChain_H

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
   * @class MonitorAlgorithmChain
   * Algorithm monitor implimentation.
   */
  class MonitorAlgorithmChain: public MonitorBase, public MonitorAlgorithmCommon {
  public:
    MonitorAlgorithmChain(const TrigCostData* _costData);
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter(const std::string& _name, Int_t _ID);
    Bool_t getIfActive(ConfKey_t _mode);
    void saveOutput();
  }; //class MonitorAlgorithmChain
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorAlgorithmChain_H
