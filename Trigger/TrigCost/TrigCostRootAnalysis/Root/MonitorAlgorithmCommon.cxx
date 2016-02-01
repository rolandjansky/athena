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
#include "../TrigCostRootAnalysis/MonitorAlgorithmCommon.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {

  // Statics
  std::vector<MonitorAlgorithmCommon::AlgsInEvent> MonitorAlgorithmCommon::m_algsInEvent = std::vector<MonitorAlgorithmCommon::AlgsInEvent>();
  Int_t MonitorAlgorithmCommon::m_eventNumber = -1;
  UInt_t MonitorAlgorithmCommon::m_level = 0;

  /**
   * AlgsInEvent is a simple structure to buffer the primary data of all algorithms in an events.
   * It may then be speedily iterated over by all algorithm monitor clients of this data.
   */
  MonitorAlgorithmCommon::AlgsInEvent::AlgsInEvent(const std::string& _algName,
        const std::string& _algClassName,
        const std::string& _seqName,
        const std::string& _chainName,
        Int_t _algNameID,
        Int_t _seqD3PDIndex,
        Int_t _algD3PDIndex) :
    m_algName(_algName),
    m_algClassName(_algClassName),
    m_seqName(_seqName),
    m_chainName(_chainName),
    m_algNameID(_algNameID),
    m_seqD3PDIndex(_seqD3PDIndex),
    m_algD3PDIndex(_algD3PDIndex) {/*Nothing here*/}

  /**
   * Populate the static m_algsInEvent vector with basic information of all algorithms run in this event, store this data in
   * AlgsInEvent structs.
   */
  void MonitorAlgorithmCommon::getAllAlgsInEvent(UInt_t _level, const TrigCostData* _costData) {
    // Nothing has changed, current list is fine
    if (m_eventNumber == _costData->getEventNumber() && m_level == _level) return;

    // Update list of algs for multiple monitors to use
    m_eventNumber = _costData->getEventNumber();
    m_level = _level;
    m_algsInEvent.clear();

    for (UInt_t _s = 0; _s < _costData->getNSequences(); ++_s) {

      // Get the name of the chain this sequence instance is in (Supplying L2 or EF helps, but is not needed)
      Int_t _chainID = _costData->getSequenceChannelCounter(_s);
      const std::string _chainName = TrigConfInterface::getHLTNameFromChainID( _chainID, _costData->getSequenceLevel(_s) );

      // Did we fail?
      if ( _chainName == Config::config().getStr(kBlankString) ) {
        Warning("MonitorAlgorithmChain::newEvent", "Skipping Chain ID %i. Cannot get name from current configuration.", _chainID);
        continue;
      }

      // Are we running over this chain?
      static Bool_t _invertFilter = Config::config().getInt(kPatternsInvert);
      if ( checkPatternNameMonitor( _chainName, _invertFilter, _costData->getSeqIsRerun(_s) ) == kFALSE ) continue;

      // Loop over all algorithms in sequence
      for (UInt_t _a = 0; _a < _costData->getNSeqAlgs(_s); ++_a) {

        Int_t _seqIndex = _costData->getSequenceIndex(_s);
        Int_t _seqAlgPos = _costData->getSeqAlgPosition(_s, _a);

        const std::string _seqName = TrigConfInterface::getHLTSeqNameFromIndex( _seqIndex );

        const std::string _algName = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos( _seqIndex, _seqAlgPos );
        const std::string _algType = TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos( _seqIndex, _seqAlgPos );
        Int_t _seqAlgNameHash = TrigConfInterface::getHLTAlgClassNameIDFromSeqIDAndAlgPos( _seqIndex, _seqAlgPos );

        m_algsInEvent.push_back( AlgsInEvent(_algName, _algType, _seqName, _chainName, _seqAlgNameHash, _s, _a) );

        //TrigCostRootAnalysis::MonitorAlgorithmCommon::AlgsInEvent::AlgsInEvent(const std::string&, const std::string&, const std::string&, const std::string&, Int_t&, UInt_t&, UInt_t&)
        //TrigCostRootAnalysis::MonitorAlgorithmCommon::AlgsInEvent::AlgsInEvent(std::string&, std::string&, std::string&, std::string&, Int_t, Int_t, Int_t)
      }
    }

  }

  /**
   * List of table entries common to all Algorithm class tables
   */
  void MonitorAlgorithmCommon::addCommonTableEntries(std::vector<MonitorBase::TableColumnFormatter>& _toSaveTable) {

    const std::string _slowText = "Calls > " + intToString( Config::config().getInt(kSlowThreshold) ) + " ms";

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Raw Active Events",
      "Raw underlying statistics on how many events in which this algorithm was executed.",
      kVarCalls, kSavePerEvent, 0, kFormatOptionUseEntries) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Active Events",
      "How many events in which this algorithm was executed.",
      kVarEventsActive, kSavePerEvent, 0) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Calls/Event",
      "Average number of calls made to this algorithm per event.",
      kVarCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter(_slowText,
      "Number of algorithm executions which were particularly slow.",
      kVarCallsSlow, kSavePerEvent, 0 ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Event Rate [Hz]",
      "Rate in this run range of events with at least one execution of this algorithm.",
      kVarEventsActive, kSavePerEvent, 2, kFormatOptionNormaliseWallTime) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Call Rate [Hz]",
      "Rate in this run range of calls to this algorithm.",
      kVarCalls, kSavePerEvent, 2, kFormatOptionNormaliseWallTime) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Alg Total Time [s]",
      "Total time for this algorithm in this range - CPU + ROS.",
      kVarTime, kSavePerCall, 2, kFormatOptionMiliSecToSec) );

    // _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Alg Total Time SPE [s]",
    //   "Total time for this algorithm in this range - CPU + ROS.",
    //   kVarTime, kSavePerEvent, 2, kFormatOptionMiliSecToSec) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Alg Total Time [%]",
      "Total time for this algorithm (CPU+ROS) as a percentage of all algorithm executions in this run range.",
      &tableFnChainGetTotalFracTime, 2 ) ); // We can re-use the Chain fn here as vars in both called "time"

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Alg Total Time/Call [ms]",
      "Average execution time per algorithm call in this run range.",
      kVarTime, kSavePerCall, kVarCalls, kSavePerEvent, 2) ); // time savePerCall normalised to savePerEvent entries

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Alg Total Time/Event [ms]",
      "Average execution time (CPU+ROS) per event for events with at least one execution in this run range.",
      kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Alg First Call Time/Event [ms]",
      "Average execution time (CPU+ROS) for the first algorithm execution in each event with at lease algorithm call in this run range.",
      kVarFirstTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("ROS Time/Event [ms]",
      "Average time waiting for ROS data per event for  events with at least one execution in this run range.",
      kVarROSTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Data Request Rate [Hz]",
      "Rate of calls to the ROS from this algorithm in this run range.",
      kVarROSCalls, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Cached ROB Rate [Hz]",
      "Rate of cached ROB fetches from this algorithm in this run range.",
      kVarROBReqs, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Cached ROB Rate [kB/s]",
      "Average size of cached ROB data fetches for this algorithm in this run range.",
      kVarROBReqSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Retrieved ROB Rate [Hz]",
      "Rate of ROB retrievals from this algorithm in this run range.",
      kVarROBRets, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( MonitorBase::TableColumnFormatter("Retrieved ROB Rate [kB/s]",
      "Average size of retrieved ROB data fetches for this algorithm in this run range.",
      kVarROBRetSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime ) );

  }

} // namespace TrigCostRootAnalysis
