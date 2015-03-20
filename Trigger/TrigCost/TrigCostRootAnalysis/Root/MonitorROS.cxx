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
#include "../TrigCostRootAnalysis/MonitorROS.h"
#include "../TrigCostRootAnalysis/CounterROB.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/ROSConfService.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT includes
#include <TError.h>

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorROS::MonitorROS(const TrigCostData* _costData) : MonitorBase(_costData, "ROS") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    allowSameIDCounters();
  }
  
  /**
   * Process new event for this monitor.
   * For the ROS, all ROS data is looped over and recorded.
   * @param _weight The event weight.
   */
  void MonitorROS::newEvent(Float_t _weight ) {

    if ( Config::config().debug() ) { 
      Int_t _N = 0;
      for (UInt_t _Rob = 0; _Rob < m_costData->getNROBs(); ++_Rob) {
        _N += m_costData->getROBDataN( _Rob);
      }
      Info("MonitorROS::newEvent", "*** Processing ROS ***  Size %i ***", _N  );
    }

    // Populate the counter collections we'll be using for this event.
    collateCounterCollectionsForEvent( m_costData->getLumi(), TrigConfInterface::getCurrentDBKey() );
    
    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;
           
      startEvent();
      
      // All looping over individual RODs to do their event monitoring can go here
      for (UInt_t _Rob = 0; _Rob < m_costData->getNROBs(); ++_Rob) {
        for (UInt_t _RobData = 0; _RobData < m_costData->getROBDataN( _Rob); ++_RobData) {
        
          Int_t _RobId = m_costData->getROBDataID(_Rob, _RobData);
          
          const std::string _RobinName = ROSConfService::rosConfService().getRobinNameFromId( (UInt_t) _RobId );
          const std::string _RosName = ROSConfService::rosConfService().getRosNameFromFromRobinName( _RobinName );

          // if ( Config::config().debug() ) {
          //   Info("MonitorROS::newEvent", "0x%x -> %s (%s)", (UInt_t) _RobId, _RobinName.c_str(), _RosName.c_str() );
          // }
          
          // Process this data for the ROBIN ...
          CounterBase* _counter =  getCounter( _counterMap, _RobinName, _RobId );
          if (_counter->getCalls() == 0) _counter->decorate(kDecType, Config::config().getStr(kROBINString) );
          _counter->addMultiID( (UInt_t) _RobId ); // Keep track of all ROB IDs we are assigning to this counter
          _counter->processEventCounter( _Rob, _RobData, _weight );

          // ... and also for the ROS this ROBIN falls under
          _counter =  getCounter( _counterMap, _RosName, _RobId );
          if (_counter->getCalls() == 0) _counter->decorate(kDecType, Config::config().getStr(kROSString));
          _counter->addMultiID( (UInt_t) _RobId );
          _counter->processEventCounter( _Rob, _RobData, _weight );
        }
      }
      
      endEvent(_weight);
    }
  }
  
  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorROS::getIfActive(ConfKey_t _mode) {
    switch(_mode) {
      case kDoAllSummary:       return kTRUE;
      case kDoKeySummary:       return kTRUE;
      case kDoLumiBlockSummary: return kTRUE;
      default: Error("MonitorROS::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(_mode).c_str() );
    }
    return kFALSE;
  }
    
  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorROS::saveOutput() {

    m_filterOutput = kTRUE; // Apply any user-specified name filter to output

    VariableOptionVector_t _toSave = m_dummyCounter->getAllHistograms();

    std::vector<TableColumnFormatter> _toSaveTable;
    _toSaveTable.push_back( TableColumnFormatter("Raw Active Events", 
      "Raw underlying statistics on the number of events in which this ROS/ROBIN was accessed.",
      kVarCalls, kSavePerEvent, 0, kFormatOptionUseEntries ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Active Events", 
      "How many events in which this sequence was executed.",
      kVarEventsActive, kSavePerEvent, 0) );

    _toSaveTable.push_back( TableColumnFormatter("Data Requests/Event",
      "Total number of ROS access requests each may contain many ROB reads.",
      kVarCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) ); 

    _toSaveTable.push_back( TableColumnFormatter("Retrieved ROBs/Event", 
      "Total number of fetched ROB calls.",
      kVarROBRets, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached ROBs/Event",
      "Total number of cached ROB calls.",
      kVarROBReqs, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Unclassified ROBs/Event", 
      "ROB calls which were marked Unclassified Ignored or Disabled.",
      kVarROBOther, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Retrieved ROB Rate [Hz]", 
      "Rate of ROB retrievals within the monitored range.",
      kVarROBRets, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached ROB Rate [Hz]", 
      "Rate of cached ROB requests within the monitored range.",
      kVarROBReqs, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Retrieved ROB Data Rate [kB/s]", 
      "Amount of data fetched from the ROBs in kB/s.",
      kVarROBRetSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached ROB Data Rate [kB/s]",
      "Amount of cached data requested from the ROBs in kB/s.",
      kVarROBReqSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    // Do these make sense?
    // _toSaveTable.push_back( TableColumnFormatter("Time Per Cached ROB [ms]",
    //   "Average time per cached ROS request.",
    //   kVarTime, kSavePerCall, kVarROBReqs, kSavePerCall, 2) );

    // _toSaveTable.push_back( TableColumnFormatter("Time Per Retrieved ROB [ms]", 
    //   "Average time per ROB retrieval.",
    //   kVarTime, kSavePerCall, kVarROBRets, kSavePerCall, 2) );

    _toSaveTable.push_back( TableColumnFormatter("Time Per Event [ms]", 
      "Average time for all requests and retrievals per event.",
      kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    // This doesn't make sense in its current form
    // _toSaveTable.push_back( TableColumnFormatter("ROBs/Data Rquest",
    //   "Average number of ROBs fetched per ROS Request involving this ROBIN/ROS.",
    //   kVarROSCalls, kSavePerCall, kVarCalls, kSavePerEvent, 2) );

    // Just do ROS first, add a requirement on "ROS" in the counter name
    Config::config().addVecEntry(kPatternsOutput, "ROS-");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSave );
    // Now go again but this time for the ROBIN, we also change the name of the monitor
    Config::config().removeVecEntry(kPatternsOutput, "ROS-");
    Config::config().addVecEntry(kPatternsOutput, "ROBIN-");
    setName( Config::config().getStr(kROBINString) );
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSave );
    Config::config().removeVecEntry(kPatternsOutput, "ROBIN-");

  }
  
  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct derived type.
   */
  CounterBase* MonitorROS::newCounter(  const std::string &_name, Int_t _ID  ) {
    return new CounterROB( m_costData, _name,  _ID, m_detailLevel );
  }
  
} // namespace TrigCostRootAnalysis
