// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorSequence_H
#define TrigCostRootAnalysis_MonitorSequence_H

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
   * @class MonitorSequence
   * Sequence monitor implimentation.
   */
  class MonitorSequence: public MonitorBase {
  public:
    MonitorSequence(const TrigCostData* _costData);
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter(const std::string& _name, Int_t _ID);
    Bool_t getIfActive(ConfKey_t _mode);
    void saveOutput();
  }; //class MonitorSequence
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorSequence_H
