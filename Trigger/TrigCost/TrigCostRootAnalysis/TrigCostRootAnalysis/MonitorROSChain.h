// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorROSChain_H
#define TrigCostRootAnalysis_MonitorROSChain_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "MonitorBase.h"
#include "MonitorROSCommon.h"

namespace TrigCostRootAnalysis {
  //Forward declaration
  class TrigCostData;

  /**
   * @class MonitorROSChain
   * ROS monitoring class, called per event if ROS monitoring is enabled.
   * responsible for global monitoring ROS request broken down per chain
   */
  class MonitorROSChain: public MonitorBase, public MonitorROSCommon {
  public:
    MonitorROSChain(const TrigCostData* costData);
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();
  private:
  }; //class MonitorROSChain
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorROSChain_H
