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
    m_timer.start();
    if ( Config::config().debug() ) Info("MonitorChain::newEvent", "*** Processing Chains ***");

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
        if ( checkPatternNameMonitor( _chainName, m_invertFilter, m_costData->getIsChainResurrected(_c) ) == kFALSE ) continue;

        CounterBase* _counter = getCounter( _counterMap, _chainName, _chainID );
        _counter->processEventCounter( _c, 0, _weight );
      }

      endEvent(_weight);
    }
    m_timer.stop();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorChain::getIfActive(ConfKey_t _mode) {
    switch(_mode) {
      case kDoAllSummary:       return kTRUE;
      case kDoKeySummary:       return kTRUE;
      case kDoLumiBlockSummary: return kTRUE;
      default: Error("MonitorChain::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(_mode).c_str() );
    }
    return kFALSE;
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
    const std::string _slowText = "Calls > " + intToString( Config::config().getInt(kSlowThreshold) ) + " ms";

    _toSaveTable.push_back( TableColumnFormatter("Raw Active Events",
      "Raw (unweighted) statistics on the number of events in which this chain was executed.",
      kVarEventsActive, kSavePerEvent, 0, kFormatOptionUseEntries) );

    _toSaveTable.push_back( TableColumnFormatter("Active Events",
      "Number of events in which this chain was executed.",
      kVarEventsActive, kSavePerEvent, 0) );

    _toSaveTable.push_back( TableColumnFormatter("Time Per Event [ms]",
      "Average execution time per event of this chain.",
      kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    // _toSaveTable.push_back( TableColumnFormatter("Effective PS Weight",
    //   "Weight applied to all events to simulate prescale.",
    //   kEffectivePrescale, kSavePerEvent, 4, kFormatOptionUseFloatDecoration ) );

    _toSaveTable.push_back( TableColumnFormatter("Execute Rate [Hz]",
      "Number of chain executions normalised to the wall time for this run range.",
      kVarEventsActive, kSavePerEvent, 4, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter(getLevelStr() + std::string(" Pass Fraction [%]"),
      "What percentage of events pass events are kept",
      kVarEventsPassed, kSavePerEvent, kVarEventsActive, kSavePerEvent, 4, kFormatOptionToPercentage) );

    _toSaveTable.push_back( TableColumnFormatter(_slowText,
      "Number of algorithm executions which were particularly slow",
      kVarEventsSlow, kSavePerEvent, 0 ) );

    _toSaveTable.push_back( TableColumnFormatter("Total Chain Time [s]",
      "Total time used by this chain for this run range.",
      kVarTime, kSavePerEvent, 2, kFormatOptionMiliSecToSec ) );

    _toSaveTable.push_back( TableColumnFormatter("Total Chain Time [%]",
      "Total chain time as a percentage of the total time of all chains in this run range.",
      &tableFnChainGetTotalFracTime, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Time Use In Rerun [%]",
      "Percentage of this chains CPU usage which comes from resurrection.",
      kVarRerunTime, kSavePerEvent, kVarTime, kSavePerEvent, 2, kFormatOptionToPercentage) );

    _toSaveTable.push_back( TableColumnFormatter("Run Agls/Event",
      "Total number of algorithms executed by this chain.",
      kVarAlgCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

    _toSaveTable.push_back( TableColumnFormatter("Cached Algs/Event",
      "Total number of algorithms which supplied a cached result to this chain.",
      kVarAlgCaches, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2 ) );

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
    return new CounterChain( m_costData, _name,  _ID, m_detailLevel, (MonitorBase*)this );
  }


} // namespace TrigCostRootAnalysis
