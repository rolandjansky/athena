// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorROSCommon_H
#define TrigCostRootAnalysis_MonitorROSCommon_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "MonitorBase.h"

namespace TrigCostRootAnalysis {
  /**
   * @class MonitorROSCommon
   * Hold common code for different ROS monitors
   */
  class MonitorROSCommon {
  public:
    StringIntSetMap_t& getROSMapping(UInt_t robReq);
    StringIntSetMap_t& getROBINMapping(UInt_t robReq);

    static void collateROSRequests(UInt_t level, const TrigCostData* costData);
  protected:
    void addCommonTableEntries(std::vector<MonitorBase::TableColumnFormatter>& toSaveTable);

    static Int_t m_eventNumber; //!< Static event number which is buffered
    static UInt_t m_level; //!< Static level which is buffered
    static std::vector< StringIntSetMap_t > m_ROSRequestStorage; //!< For each ROS request, buffer which ROS are hit
                                                                 // (integrate over ROBINs)
    static std::vector< StringIntSetMap_t > m_ROBINRequestStorage; //!< For each ROS request, buffer which ROBINs are
                                                                   // hit
  }; //class MonitorROSCommon
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorROSCommon_H
