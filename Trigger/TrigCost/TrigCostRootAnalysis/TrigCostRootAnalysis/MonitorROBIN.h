// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorROBIN_H
#define TrigCostRootAnalysis_MonitorROBIN_H

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
   * @class MonitorROBIN
   * ROS monitoring class, called per event if ROS monitoring is enabled.
   * responsible for lower level monitoring at the per ROBIN level
   */
  class MonitorROBIN : public MonitorBase, public MonitorROSCommon {

   public:

    MonitorROBIN(const TrigCostData* _costData);
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter( const std::string &_name, Int_t _ID );
    Bool_t getIfActive(ConfKey_t _mode);
    void saveOutput();

   private:


  }; //class MonitorROBIN

} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorROBIN_H
