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
  class MonitorROBIN: public MonitorBase, public MonitorROSCommon {
  public:
    // StringIntSetMap_t& getROBINMapping(UInt_t robReq);
    //static void collateROBINRequests(UInt_t level, const TrigCostData* costData);

    MonitorROBIN(const TrigCostData* costData);
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();

    /*
       protected:
       static Int_t m_eventNumber; //!< Static event number which is buffered
       static UInt_t m_level; //!<Static level which is buffered
       static std::vector< StringIntSetMap_t > m_ROBINRequestStorage; //!< For each ROS request, buffer which ROBINs are
          hit
     */
  private:
  }; //class MonitorROBIN
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorROBIN_H
