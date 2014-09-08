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
#include "../TrigCostRootAnalysis/MonitorChain.h"
#include "../TrigCostRootAnalysis/CounterChain.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorChain::MonitorChain(const TrigCostData* _costData) : MonitorBase(_costData, "Chain") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
  }
  
  /**
   * Process new event for this monitor.
   * For the chain monitor, all chains present in this event are processed and recorded.
   * @param _weight The event weight.
   */
  void MonitorChain::newEvent(Float_t _weight) {
  
    if ( Config::config().debug() ) Info("MonitorChain::newEvent", "*** Processing Chains ***");
    
    // Populate the counter collections we'll be using for this event.
    collateCounterCollectionsForEvent( m_costData->getLumi(), TrigConfInterface::getCurrentDBKey() );
    
    //Now loop over the counter collections;
    
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;
      
      startEvent();
      
      // Loop over all chains.
      for (UInt_t _c = 0; _c < m_costData->getNChains(); ++_c) {
      
        if ( (UInt_t) m_costData->getChainLevel(_c) != getLevel() ) {
          //Info("MonitorChain::newEvent", "Trigger mis-match %i-chain in %i-MonitorChain", m_costData->getChainLevel(_c), getLevel() );
          continue;
        }
        
        // Get the name of the chain (Supplying L2 or EF helps, but is not needed)
        Int_t _chainID = m_costData->getChainID(_c);
        const std::string _chainName = TrigConfInterface::getHLTNameFromChainID( _chainID, m_costData->getChainLevel(_c) );
        
        // Did we fail?
        if ( _chainName == Config::config().getStr(kBlankString) ) {
          Warning("MonitorChain::newEvent", "Skipping Chain ID %i. Cannot get name from current configuration.", _chainID);
          continue;
        }
        
        // Are we running over this chain?
        if ( checkPatternNameMonitor( _chainName ) == kFALSE ) continue;
        
        CounterBase* _counter = getCounter( _counterMap, _chainName, _chainID );
        _counter->processEventCounter( _c, 0, _weight );
      }
      
      endEvent();
    }
  }
  
  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorChain::saveOutput() {
  
    m_filterOutput = kTRUE; // Apply any user-specified name filter to output
    
    // Specify what plots we wish to save from the counters
    VariableOptionVector_t _toSavePlots = m_dummyCounter->getAllHistograms();
    sharedHistogramOutputRoutine( _toSavePlots );
    
    std::vector<TableColumnFormatter> _toSaveTable;
    const std::string _slowText = "Calls > " + intToString( Config::config().getInt(kSlowEventThreshold) ) + " ms";
    
    _toSaveTable.push_back( TableColumnFormatter("Active Events", 
      "Number of events in which this chain was executed.",
      kVarEventsPassed, kSavePerEvent, 0, kFormatOptionUseEntries) );

    _toSaveTable.push_back( TableColumnFormatter("Passed Events", 
      "Number of events in which this chain passed the physics requirement.",
      kVarEventsPassed, kSavePerEvent, 0 ) );

    _toSaveTable.push_back( TableColumnFormatter("Time Per Event [ms]", 
      "Average execution time per event of this chain.",
      kVarTime, kSavePerEvent, 2, kFormatOptionNormaliseEntries ) );

    _toSaveTable.push_back( TableColumnFormatter("Chain Rate [Hz]", 
      "Events passed normalised to the wall time for this run range.",
      kVarEventsPassed, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter(_slowText, 
      "Number of algorithm executions which were particularly slow",
      kVarEventsSlow, kSavePerEvent, 0 ) );

    _toSaveTable.push_back( TableColumnFormatter("Total Chain Time [s]", 
      "Total time used by this chain for this run range.",
      kVarTime, kSavePerEvent, 2, kFormatOptionMiliSecToSec ) );

    _toSaveTable.push_back( TableColumnFormatter("Total Chain Time [%]", 
      "Total chain time as a percentage of the total time of all chains in this run range.",
      &tableFnChainGetTotalFracTime, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Run Agls", 
      "Total number of algorithms executed by this chain.",
      kVarAlgCalls, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached Algs", 
      "Total number of algorithms which supplied a cached result to this chain.",
      kVarAlgCaches, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached ROB Rate [Hz]", 
      "Number of cached ROBs requested by this chain normalised to the run range.",
      kVarROBReqs, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached ROB Rate [kB/s]", 
      "Size of cached ROBs requested by this chain normalised to the run range.",
      kVarROBReqSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Retrieved ROB Rate [Hz]", 
      "Number of ROBs retrieved by this chain normalised to the run range.",
      kVarROBRets, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Retrieved ROB Rate [kB/s]", 
      "Size of the ROBs retrieved by this chain normalised to the run range.",
      kVarROBRetSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );   

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
  CounterBase* MonitorChain::newCounter(  const std::string &_name, Int_t _ID  ) {
    return new CounterChain( m_costData, _name,  _ID, m_detailLevel );
  }
  
  
} // namespace TrigCostRootAnalysis
