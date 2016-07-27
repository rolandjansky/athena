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
  StringIntSetMap_t& MonitorROSCommon::getROSMapping(UInt_t _robReq) {
    return m_ROSRequestStorage.at(_robReq);
  }
  
  StringIntSetMap_t& MonitorROSCommon::getROBINMapping(UInt_t _robReq) {
    return m_ROBINRequestStorage.at(_robReq);
  }


  /**
   * Each ROS request comes as a series of ROBIN IDs
   * To not overcount the ROS request rate, the ROBINs in a single
   * request which come from the same ROS should be combined together
   * in the ROSCounter. This function precalculats all these connections
   * for all of the monitors deriving from ROSCommon
   */
  void MonitorROSCommon::collateROSRequests(UInt_t _level, const TrigCostData* _costData) {
    // Nothing has changed, current list is fine
    if (m_eventNumber == _costData->getEventNumber() && m_level == _level) return;

    // Update static cached storage for multiple monitors to use
    m_eventNumber = _costData->getEventNumber();
    m_level = _level;
    for (UInt_t _n = 0; _n < m_ROSRequestStorage.size(); ++_n) {
      for (StringIntSetMapNonConstIt_t _reqIt = m_ROSRequestStorage.at(_n).begin(); _reqIt != m_ROSRequestStorage.at(_n).end(); ++_reqIt) {
        (*_reqIt).second.clear();
      }
      m_ROSRequestStorage.at(_n).clear();
    }
    {
      std::vector< StringIntSetMap_t > scopedVector;
      scopedVector.swap( m_ROSRequestStorage );
    }
    // Out of scope, contents deleted
    
    for (UInt_t _n1 = 0; _n1 < m_ROBINRequestStorage.size(); ++_n1) {
      for (StringIntSetMapNonConstIt_t _reqIt1 = m_ROBINRequestStorage.at(_n1).begin(); _reqIt1 != m_ROBINRequestStorage.at(_n1).end(); ++_reqIt1) {
        (*_reqIt1).second.clear();
      }
      m_ROBINRequestStorage.at(_n1).clear();
    }
    {
      std::vector< StringIntSetMap_t > scopedVector1;
      scopedVector1.swap( m_ROBINRequestStorage );
    }
    // Out of scope, contents deleted


    //Copied above for m_ROBINRequestStorage
    

    // Store the per ROS request mappings
    for (UInt_t _rob = 0; _rob < _costData->getNROBs(); ++_rob) {
      StringIntSetMap_t _ROSNameToROBINLocs;
      StringIntSetMap_t _ROBINMapping;
      for (UInt_t _robData = 0; _robData < _costData->getROBDataN( _rob ); ++_robData) {

        Int_t _robId = _costData->getROBDataID(_rob, _robData);
        const std::string _robinName = ROSConfService::rosConfService().getRobinNameFromId( (UInt_t) _robId );
        const std::string _rosName = ROSConfService::rosConfService().getRosNameFromFromRobinName( _robinName );

        // Map the ROS name to the D3PD index of the ROBINs in this request which are associated to this ROS
        _ROSNameToROBINLocs[ _rosName ].insert( (Int_t) _robData );
	      _ROBINMapping[ _robinName ].insert( (Int_t) _robData );
      }
      m_ROSRequestStorage.push_back( _ROSNameToROBINLocs );
      m_ROBINRequestStorage.push_back( _ROBINMapping );
    }

  }

  /**
   * List of table entries common to the different ROS monitors. Avoid code dupe
   */
  void MonitorROSCommon::addCommonTableEntries(std::vector<MonitorBase::TableColumnFormatter>& _toSaveTable) {

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Raw Active Events",
      "Raw underlying statistics on the number of events in which this ROS was accessed.",
      kVarCalls, kSavePerEvent, 0, kFormatOptionUseEntries ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Active Events",
      "How many events in which this sequence was executed.",
      kVarEventsActive, kSavePerEvent, 0) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Data Requests/Event",
      "Total number of ROS access requests each may contain many ROB reads.",
      kVarCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Retrieved ROBs/Event",
      "Total number of fetched ROB calls.",
      kVarROBRets, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Cached ROBs/Event",
      "Total number of cached ROB calls.",
      kVarROBReqs, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Retrieved ROB Rate [Hz]",
      "Rate of ROB retrievals within the monitored range.",
      kVarROBRets, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Cached ROB Rate [Hz]",
      "Rate of cached ROB requests within the monitored range.",
      kVarROBReqs, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Retrieved ROB Data Rate [kB/s]",
      "Amount of data fetched from the ROBs in kB/s.",
      kVarROBRetSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Cached ROB Data Rate [kB/s]",
      "Amount of cached data requested from the ROBs in kB/s.",
      kVarROBReqSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    // Do these make sense?
    // _toSaveTable.push_back( TableColumnFormatter("Time Per Cached ROB [ms]",
    //   "Average time per cached ROS request.",
    //   kVarTime, kSavePerCall, kVarROBReqs, kSavePerCall, 2) );

    // _toSaveTable.push_back( TableColumnFormatter("Time Per Retrieved ROB [ms]",
    //   "Average time per ROB retrieval.",
    //   kVarTime, kSavePerCall, kVarROBRets, kSavePerCall, 2) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Time Per Event [ms]",
      "Average time for all requests and retrievals per event.",
      kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Unclassified ROBs/Event",
      "ROB calls which were flagged unclassified.",
      kVarROBUnclassified, kSavePerEvent, kVarEventsActive, kSavePerEvent, 4 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Disabled ROBs/Event",
      "ROB calls which were flagged as disabled.",
      kVarROBDisabled, kSavePerEvent, kVarEventsActive, kSavePerEvent, 4 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Ignored ROBs/Event",
      "ROB calls which were flagged as ignored.",
      kVarROBIgnored, kSavePerEvent, kVarEventsActive, kSavePerEvent, 4 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Prefetched ROBs/Event",
      "ROB calls which were flagged as prefetched.",
      kVarROBPrefetched, kSavePerEvent, kVarEventsActive, kSavePerEvent, 4 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Not OK ROBs/Event",
      "ROB calls in which the is OK bit was false.",
      kVarROBNotOK, kSavePerEvent, kVarEventsActive, kSavePerEvent, 4 ) );


    // This doesn't make sense in its current form
    // _toSaveTable.push_back( TableColumnFormatter("ROBs/Data Rquest",
    //   "Average number of ROBs fetched per ROS Request involving this ROBIN/ROS.",
    //   kVarROSCalls, kSavePerCall, kVarCalls, kSavePerEvent, 2) );



  }


} // namespace TrigCostRootAnalysis
