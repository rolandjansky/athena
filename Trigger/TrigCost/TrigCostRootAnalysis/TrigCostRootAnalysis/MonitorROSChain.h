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
    MonitorROSChain(const TrigCostData* _costData);
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter(const std::string& _name, Int_t _ID);
    Bool_t getIfActive(ConfKey_t _mode);
    void saveOutput();
  private:
  }; //class MonitorROSChain
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorROSChain_H
