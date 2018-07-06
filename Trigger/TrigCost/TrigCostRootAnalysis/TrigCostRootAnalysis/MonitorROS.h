// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorROS_H
#define TrigCostRootAnalysis_MonitorROS_H

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
   * @class MonitorROS
   * ROS monitoring class, called per event if ROS monitoring is enabled.
   * responsible for global ROS monitoring
   */
  class MonitorROS: public MonitorBase, public MonitorROSCommon {
  public:
    MonitorROS(const TrigCostData* costData);
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();
  private:
  }; //class MonitorROS
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorROS_H
