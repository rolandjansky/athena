// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <utility> //std::pair
#include <iostream>
#include <assert.h>

//Local include(s):
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/MonitorROSCommon.h"
#include "../TrigCostRootAnalysis/MonitorBase.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/ROSConfService.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

// ROOT includes
#include <TError.h>

namespace TrigCostRootAnalysis {
  // Statics
  Int_t MonitorROSCommon::m_eventNumber = -1;
  UInt_t MonitorROSCommon::m_level = 0;
  std::vector< StringIntSetMap_t > MonitorROSCommon::m_ROSRequestStorage = std::vector< StringIntSetMap_t >();
  std::vector< StringIntSetMap_t > MonitorROSCommon::m_ROBINRequestStorage = std::vector< StringIntSetMap_t >();

  /**
   * Get the cache data for a ROS request
   */
  StringIntSetMap_t& MonitorROSCommon::getROSMapping(UInt_t robReq) {
    return m_ROSRequestStorage.at(robReq);
  }

  StringIntSetMap_t& MonitorROSCommon::getROBINMapping(UInt_t robReq) {
    return m_ROBINRequestStorage.at(robReq);
  }

  /**
   * Each ROS request comes as a series of ROBIN IDs
   * To not overcount the ROS request rate, the ROBINs in a single
   * request which come from the same ROS should be combined together
   * in the ROSCounter. This function precalculats all these connections
   * for all of the monitors deriving from ROSCommon
   */
  void MonitorROSCommon::collateROSRequests(UInt_t level, const TrigCostData* costData) {
    // Nothing has changed, current list is fine
    if (m_eventNumber == costData->getEventNumber() && m_level == level) return;

    // Update static cached storage for multiple monitors to use
    m_eventNumber = costData->getEventNumber();
    m_level = level;
    for (UInt_t n = 0; n < m_ROSRequestStorage.size(); ++n) {
      for (StringIntSetMapNonConstIt_t reqIt = m_ROSRequestStorage.at(n).begin();
           reqIt != m_ROSRequestStorage.at(n).end(); ++reqIt) {
        (*reqIt).second.clear();
      }
      m_ROSRequestStorage.at(n).clear();
    }

    for (UInt_t n = 0; n < m_ROBINRequestStorage.size(); ++n) {
      for (StringIntSetMapNonConstIt_t reqIt = m_ROBINRequestStorage.at(n).begin();
           reqIt != m_ROBINRequestStorage.at(n).end(); ++reqIt) {
        (*reqIt).second.clear();
      }
      m_ROBINRequestStorage.at(n).clear();
    }

    {
      std::vector< StringIntSetMap_t > scopedVector;
      std::vector< StringIntSetMap_t > otherScopedVector;
      scopedVector.swap(m_ROSRequestStorage);
      otherScopedVector.swap(m_ROBINRequestStorage);
    }

    // Out of scope, contents deleted

    for (UInt_t n1 = 0; n1 < m_ROBINRequestStorage.size(); ++n1) {
      for (StringIntSetMapNonConstIt_t reqIt1 = m_ROBINRequestStorage.at(n1).begin();
           reqIt1 != m_ROBINRequestStorage.at(n1).end(); ++reqIt1) {
        (*reqIt1).second.clear();
      }
      m_ROBINRequestStorage.at(n1).clear();
    }
    {
      std::vector< StringIntSetMap_t > scopedVector1;
      scopedVector1.swap(m_ROBINRequestStorage);
    }
    // Out of scope, contents deleted


    //Copied above for m_ROBINRequestStorage


    // Store the per ROS request mappings
    for (UInt_t rob = 0; rob < costData->getNROBs(); ++rob) {
      StringIntSetMap_t ROSNameToROBINLocs;

      StringIntSetMap_t ROBINNameToLoc;

      for (UInt_t robData = 0; robData < costData->getROBDataN(rob); ++robData) {
        Int_t robId = costData->getROBDataID(rob, robData);
        const std::string robinName = ROSConfService::rosConfService().getRobinNameFromId((UInt_t) robId);
        const std::string rosName = ROSConfService::rosConfService().getRosNameFromFromRobinName(robinName);

        // Map the ROS name to the D3PD index of the ROBINs in this request which are associated to this ROS
        ROSNameToROBINLocs[ rosName ].insert((Int_t) robData);
        ROBINNameToLoc[ robinName ].insert((Int_t ) robData);
      }
      m_ROSRequestStorage.push_back(ROSNameToROBINLocs);
      m_ROBINRequestStorage.push_back(ROBINNameToLoc);
    }
  }

  /**
   * List of table entries common to the different ROS monitors. Avoid code dupe
   */
  void MonitorROSCommon::addCommonTableEntries(std::vector<MonitorBase::TableColumnFormatter>& toSaveTable) {
    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Raw Active Events",
                                                             "Raw underlying statistics on the number of events in which this ROS was accessed.",
                                                             kVarCalls, kSavePerEvent, 0, kFormatOptionUseEntries));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Active Events",
                                                             "How many events in which this sequence was executed.",
                                                             kVarEventsActive, kSavePerEvent, 0));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Data Requests/Event",
                                                             "Total number of ROS access requests each may contain many ROB reads.",
                                                             kVarCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent,
                                                             2));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Retrieved ROBs/Event",
                                                             "Total number of fetched ROB calls.",
                                                             kVarROBRets, kSavePerEvent, kVarEventsActive,
                                                             kSavePerEvent, 2));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Cached ROBs/Event",
                                                             "Total number of cached ROB calls.",
                                                             kVarROBReqs, kSavePerEvent, kVarEventsActive,
                                                             kSavePerEvent, 2));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Retrieved ROB Rate [Hz]",
                                                             "Rate of ROB retrievals within the monitored range.",
                                                             kVarROBRets, kSavePerEvent, 2,
                                                             kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Cached ROB Rate [Hz]",
                                                             "Rate of cached ROB requests within the monitored range.",
                                                             kVarROBReqs, kSavePerEvent, 2,
                                                             kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Retrieved ROB Data Rate [kB/s]",
                                                             "Amount of data fetched from the ROBs in kB/s.",
                                                             kVarROBRetSize, kSavePerEvent, 2,
                                                             kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Cached ROB Data Rate [kB/s]",
                                                             "Amount of cached data requested from the ROBs in kB/s.",
                                                             kVarROBReqSize, kSavePerEvent, 2,
                                                             kFormatOptionNormaliseWallTime));

    // Do these make sense?
    // toSaveTable.push_back( TableColumnFormatter("Time Per Cached ROB [ms]",
    //   "Average time per cached ROS request.",
    //   kVarTime, kSavePerCall, kVarROBReqs, kSavePerCall, 2) );

    // toSaveTable.push_back( TableColumnFormatter("Time Per Retrieved ROB [ms]",
    //   "Average time per ROB retrieval.",
    //   kVarTime, kSavePerCall, kVarROBRets, kSavePerCall, 2) );

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Time Per Event [ms]",
                                                             "Average time for all requests and retrievals per event.",
                                                             kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent,
                                                             2));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Unclassified ROBs/Event",
                                                             "ROB calls which were flagged unclassified.",
                                                             kVarROBUnclassified, kSavePerEvent, kVarEventsActive,
                                                             kSavePerEvent, 4));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Disabled ROBs/Event",
                                                             "ROB calls which were flagged as disabled.",
                                                             kVarROBDisabled, kSavePerEvent, kVarEventsActive,
                                                             kSavePerEvent, 4));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Ignored ROBs/Event",
                                                             "ROB calls which were flagged as ignored.",
                                                             kVarROBIgnored, kSavePerEvent, kVarEventsActive,
                                                             kSavePerEvent, 4));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Prefetched ROBs/Event",
                                                             "ROB calls which were flagged as prefetched.",
                                                             kVarROBPrefetched, kSavePerEvent, kVarEventsActive,
                                                             kSavePerEvent, 4));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Not OK ROBs/Event",
                                                             "ROB calls in which the is OK bit was false.",
                                                             kVarROBNotOK, kSavePerEvent, kVarEventsActive,
                                                             kSavePerEvent, 4));


    // This doesn't make sense in its current form
    // toSaveTable.push_back( TableColumnFormatter("ROBs/Data Rquest",
    //   "Average number of ROBs fetched per ROS Request involving this ROBIN/ROS.",
    //   kVarROSCalls, kSavePerCall, kVarCalls, kSavePerEvent, 2) );
  }
} // namespace TrigCostRootAnalysis
