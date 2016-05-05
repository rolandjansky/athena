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

//Local include(s):
#include "../TrigCostRootAnalysis/MonitorSliceCPU.h"
#include "../TrigCostRootAnalysis/CounterSliceCPU.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorSliceCPU::MonitorSliceCPU(const TrigCostData* _costData) : MonitorBase(_costData, "SliceCPU") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    allowSameIDCounters();
  }

  /**
   * Process new event for this monitor.
   * For each alg, get its chain and the chain's first group. Use this as the counter type
   * @param _weight The event weight.
   */
  void MonitorSliceCPU::newEvent(Float_t _weight) {
    m_timer.start();
    if ( Config::config().debug() ) Info("MonitorSliceCPU::newEvent", "*** Processing Algorithms ***");

    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;

      startEvent();

      // Loop over all chains.
      for (UInt_t _c = 0; _c < m_costData->getNChains(); ++_c) {

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
        
        std::string _chainGroup;
        if (TrigConfInterface::getNHLTGroupNamesFromChainID( _chainID ) > 0) {
          _chainGroup = TrigConfInterface::getHLTGroupNameFromChainID(_chainID, 0);
        } else {
          _chainGroup = Config::config().getStr(kUnknownString);
          if (Config::config().getDisplayMsg(kMsgNoGroup) == kTRUE) Warning("MonitorSliceCPU::newEvent", "Chain %s has no group", _chainName.c_str());
        }

        CounterBase* _counter = getCounter( _counterMap, _chainGroup, 0 );
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
  Bool_t MonitorSliceCPU::getIfActive(ConfKey_t _mode) {
    switch(_mode) {
      case kDoAllSummary:       return kTRUE;
      case kDoKeySummary:       return kTRUE;
      case kDoLumiBlockSummary: return kTRUE;
      default: Error("MonitorSliceCPU::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(_mode).c_str() );
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorSliceCPU::saveOutput() {

    m_filterOutput = kTRUE; // Apply any user-specified name filter to output

    VariableOptionVector_t _toSavePlots = m_dummyCounter->getAllHistograms();
    std::vector<TableColumnFormatter> _toSaveTable;

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Event Rate [Hz]",
      "Rate in this run range of events with at least one execution of this algorithm.",
      kVarEventsActive, kSavePerEvent, 2, kFormatOptionNormaliseWallTime) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Group Total Time [s]",
      "Total time for a chain execution in this group.",
      kVarTime, kSavePerCall, 2, kFormatOptionMiliSecToSec) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Group Total Time [%]",
      "Total time for chain execution in this group as a percentage of all algorithm executions in this run range.",
      &tableFnChainGetTotalFracTime, 2 ) ); // We can re-use the Chain fn here as vars in both called "time"

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Group Total Time/Event [ms]",
      "Average execution time for chains in this group per event for events with at least one executed chain in this run range.",
      kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorSliceCPU::newCounter( const std::string &_name, Int_t _ID ) {
    return new CounterSliceCPU( m_costData, _name,  _ID, m_detailLevel, (MonitorBase*)this );
  }

} // namespace TrigCostRootAnalysis
