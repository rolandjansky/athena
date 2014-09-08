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
#include <vector>

// Local include(s):
#include "../TrigCostRootAnalysis/MonitorROI.h"
#include "../TrigCostRootAnalysis/CounterROI.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorROI::MonitorROI(const TrigCostData* _costData) : MonitorBase(_costData, "ROI") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
  }
  
  /**
   * Process new event for this monitor.
   * We have one counter per ROI type and a global one
   * @param _weight The event weight.
   */
  void MonitorROI::newEvent(Float_t _weight) {
  
    if ( Config::config().debug() ) Info("MonitorROI::newEvent", "*** Processing ROI ***");
    
    // Populate the counter collections we'll be using for this event.
    collateCounterCollectionsForEvent( m_costData->getLumi(), TrigConfInterface::getCurrentDBKey() );
    
    //Now loop over the counter collections;
    
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;
      
      startEvent();
      
      // Loop over all chains.
      for (UInt_t _r = 0; _r < m_costData->getNRoIs(); ++_r) {

        CounterBase* _counter =  getCounter(_counterMap, m_costData->getRoITypeString(_r), 0);

        if (_counter != 0) {
          _counter->processEventCounter( _r, 0, _weight );
        }

        // Call another counter on all RoIs to get the global picture
        _counter = getCounter(_counterMap, Config::config().getStr(kAllROIString), 6);
        _counter->processEventCounter( _r, 0, _weight );

      }

      endEvent();

    }
  }
  
  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorROI::saveOutput() {
  
    m_filterOutput = kFALSE; // Apply any user-specified name filter to output
    
    // Specify what plots we wish to save from the counters
    VariableOptionVector_t _toSavePlots = m_dummyCounter->getAllHistograms();
    sharedHistogramOutputRoutine( _toSavePlots );

    std::vector<TableColumnFormatter> _toSaveTable;
    _toSaveTable.push_back( TableColumnFormatter("ROIs",
      "Total number of ROIs of this type.", 
      kVarCalls, kSavePerEvent, 0 ) );

    _toSaveTable.push_back( TableColumnFormatter("ROIs/Event", 
      "Average number of ROIs of this type per event.", 
      kVarCalls, kSavePerEvent, 4, kFormatOptionNormaliseEntries ) );

    _toSaveTable.push_back( TableColumnFormatter("L1Thresholds/ROI", 
      "Average number of L1 Thresholds per ROI of this type.",
       kVarL1Thresh, kSavePerCall, 2, kFormatOptionNormaliseEntries ) );  

    sharedTableOutputRoutine( _toSaveTable );
  }


  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorROI::newCounter(  const std::string &_name, Int_t _ID  ) {
    return new CounterROI( m_costData, _name,  _ID, m_detailLevel );
  }
  
  
} // namespace TrigCostRootAnalysis
