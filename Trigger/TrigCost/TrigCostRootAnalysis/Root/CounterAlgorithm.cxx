// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <float.h> // FLT_MAX
#include <assert.h>

// ROOT include(s):
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterAlgorithm.h"
#include "../TrigCostRootAnalysis/ROSConfService.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

namespace TrigCostRootAnalysis {
  Float_t CounterAlgorithm::s_eventTimeExecute = 0.;

  /**
   * Algorithm counter constructor. Sets values of internal variables and sets up data store.
   * @param _name Const ref to algorithm's name
   * @param _ID Algorithms ID number. As an algorithm has two ID numbers, we take a hash of its name as its ID.
   */
  CounterAlgorithm::CounterAlgorithm(const TrigCostData* _costData, const std::string& _name, Int_t _ID,
                                     UInt_t _detailLevel, MonitorBase* _parent) :
    CounterBase(_costData, _name, _ID, _detailLevel, _parent),
    m_firstAlgStartTime(FLT_MAX),
    m_firstAlgTime(0.) {
    //m_firstAlgTimeWeight(0.),
    //m_eventWeight(1.)
    if (m_detailLevel == 0) {
      // Detail 0 is no histograms and additional data cached from first execution
      // Used for full-detailed event summary
      m_dataStore.setHistogramming(kFALSE);
    }

    m_dataStore.newVariable(kVarCalls).setSavePerEvent();
    m_dataStore.newVariable(kVarCallsSlow).setSavePerEvent();
    m_dataStore.newVariable(kVarEventsActive).setSavePerEvent();

    // Time variables
    m_dataStore.newVariable(kVarTime)
     .setSavePerCall("Algorithm Total Time Per Call;Algorithm Total Time per Call [ms];Calls")
     .setSavePerEvent("Algorithm Total Time Per Event;Algorithm Total Time per Event [ms];Events")
     .setSavePerEventFraction("Fractional Algorithm Total Time;Algorithm Total Time/Event Total Time;Events");
    m_dataStore.newVariable(kVarTimeExec)
     .setSavePerCall("Algorithm Exec Time Per Call (no cached);Algorithm Exec Time per Call [ms];Calls");
    m_dataStore.newVariable(kVarFirstTime)
     .setSavePerEvent("Time For Algorithm First Call Per Event;First Call Total Algorithm Time [ms];Events");
    m_dataStore.newVariable(kVarCPUTime)
     .setSavePerCall("Algorithm CPU Time Per Call;Algorithm CPU Time [ms];Calls")
     .setSavePerEvent("Algorithm CPU Time Per Event;Algorithm CPU Time [ms];Events");
    m_dataStore.newVariable(kVarROSTime)
     .setSavePerCall("Algorithm ROB Time Per Call;Algorithm ROB Time [ms];Calls")
     .setSavePerEvent("Algorithm ROB Time Per Event;Algorithm ROB Time [ms];Events");

    // Cache status
    m_dataStore.newVariable(kVarAlgCalls)
     .setSavePerEvent("Number Of Algorithm Executions Per Event;Algorithm Executions per Event;Events");
    m_dataStore.newVariable(kVarAlgCaches)
     .setSavePerEvent("Number Of Cached Algorithms Per Event;Cached Algorithms per Event;Events");

    // ROS status
    m_dataStore.newVariable(kVarROSCalls).setSavePerCall().setSavePerEvent();

    m_dataStore.newVariable(kVarROBReqs)
     .setSavePerCall("Number Of Cached ROBs Per Call;Cached ROBs;Calls")
     .setSavePerEvent("Number Of Cached ROBs Per Event;Cached ROBs;Events");

    m_dataStore.newVariable(kVarROBRets)
     .setSavePerCall("Number Of Retrieved ROBs Per Call;Retrieved ROBs;Calls")
     .setSavePerEvent("Number Of Retrieved ROBs Per Event;Retrieved ROBS;Events");

    m_dataStore.newVariable(kVarROBReqSize)
     .setSavePerCall("Size Of Cached ROBs Per Call;Cached ROBs Size [kB];Calls")
     .setSavePerEvent("Size Of Cached ROBs Per Event;Cached ROBs Size [kB];Events");

    m_dataStore.newVariable(kVarROBRetSize)
     .setSavePerCall("Size Of Retrieved ROBs Per Call;Retrieved ROBs Size [kB];Calls")
     .setSavePerEvent("Size Of Retrieved ROBs Per Event;Retrieved ROBs Size [kB];Events");

    m_dataStore.newVariable(kVarROBOther).setSavePerEvent();
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterAlgorithm::~CounterAlgorithm() {
  }

  /**
   * Reset per-event counter(s). This is a static so this needs to only be called on one instance of this class,
   * not all instances.
   */
  void CounterAlgorithm::startEvent() {
    s_eventTimeExecute = 0;
  }

  /**
   * Perform monitoring of a single algorithm call within an event.
   * @param _e Sequence index in D3PD.
   * @param _f Location of algorithm within parent sequence.
   * @param _weight Event weight.
   */
  void CounterAlgorithm::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;

    // After some thought - this just doesn't work
    //Float_t _prescaleFactor = getPrescaleFactor(_e);
    //_weight *= _prescaleFactor;

    // Special case. If we are running as part of the FullEvent monitor, we should keep weight=1 to make the output
    // readable
    if (m_detailLevel == 0 && m_calls == 1) _weight = 1.;

    if (isZero(_weight) == kTRUE) {
      return;
    }

    if (Config::config().debug()) debug(_e, _f);

    // After some thought - this just doesn't work
    // const std::string _myChain = TrigConfInterface::getHLTNameFromChainID(
    // m_costData->getSequenceChannelCounter(_e));
    // if ( m_chainsSeen.count( _myChain ) == 0) {
    //   m_eventWeight *= (1. - _prescaleFactor);
    //   m_chainsSeen.insert( _myChain );
    // }

    m_dataStore.store(kVarCalls, 1., _weight);

    m_dataStore.store(kVarAlgCalls, m_costData->getSeqAlgIsCalled(_e, _f), _weight);
    m_dataStore.store(kVarAlgCaches, m_costData->getSeqAlgIsCached(_e, _f), _weight);

    m_dataStore.store(kVarROSCalls, m_costData->getSeqAlgROSCalls(_e, _f), _weight);
    if (m_costData->getSeqAlgROBRequests(_e, _f) != 0) {
      m_dataStore.store(kVarROBReqs, m_costData->getSeqAlgROBRequests(_e, _f), _weight);
      m_dataStore.store(kVarROBReqSize, m_costData->getSeqAlgROBRequestSize(_e, _f), _weight);
    }
    if (m_costData->getSeqAlgROBRetrievals(_e, _f) != 0) {
      m_dataStore.store(kVarROBRets, m_costData->getSeqAlgROBRetrievals(_e, _f), _weight);
      m_dataStore.store(kVarROBRetSize, m_costData->getSeqAlgROBRetrievalSize(_e, _f), _weight);
    }
    if (m_costData->getSeqAlgROBOthers(_e, _f) != 0) {
      m_dataStore.store(kVarROBOther, m_costData->getSeqAlgROBOthers(_e, _f), _weight);
    }

    if (m_costData->getSeqAlgTimer(_e, _f) > Config::config().getInt(kSlowThreshold)) {
      m_dataStore.store(kVarCallsSlow, 1., _weight);
    }

    // We record separately the the time of the first call in the event
    if (m_costData->getSeqAlgTimeStart(_e, _f) < m_firstAlgStartTime) {
      m_firstAlgStartTime = m_costData->getSeqAlgTimeStart(_e, _f);
      m_firstAlgTime = m_costData->getSeqAlgTimer(_e, _f);
      //m_firstAlgTimeWeight = _weight;
    }

    m_dataStore.store(kVarTime, m_costData->getSeqAlgTimer(_e, _f), _weight);

    // Here we only do the timing if the alg was callled NOT cached
    if (m_costData->getSeqAlgIsCalled(_e, _f) == 1) m_dataStore.store(kVarTimeExec, m_costData->getSeqAlgTimer(_e,
                                                                                                               _f),
                                                                      _weight);

    Float_t _ROSTime = m_costData->getSeqAlgROSTime(_e, _f);
    if (isZero(_ROSTime) == kFALSE) {
      m_dataStore.store(kVarROSTime, m_costData->getSeqAlgROSTime(_e, _f), _weight);
      m_dataStore.store(kVarCPUTime, m_costData->getSeqAlgTimer(_e, _f) - _ROSTime, _weight);
    }
    // Gather time statistics for this algorithm call
    s_eventTimeExecute += m_costData->getSeqAlgTimer(_e, _f) * _weight;

    // If calling this counter exactly once on a alg single execution - save some extra levels details.
    // This allows for a full-scale debug of a single event execution.
    if (m_detailLevel == 0 && m_calls == 1) fullExecutionInformation(_e, _f, _weight);
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * For an algorithm, we need to figure out the chain which the alg belongs to and then check it's enabled and then
   * scale by its L1 prescale
   * @return Multiplicative weighting factor
   */
  Double_t CounterAlgorithm::getPrescaleFactor(UInt_t _e) {
    return TrigXMLService::trigXMLService().getHLTCostWeightingFactor(
      TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(_e),
                                               m_costData->getSequenceLevel(_e)));
  }

  /**
   * @param _e Sequence index in D3PD.
   * @param _f Location of algorithm within parent sequence
   * @return How much time has passed in seconds from the beginning of the event to this alg's execution
   */
  Float_t CounterAlgorithm::getElapsedTime(UInt_t _e, UInt_t _f) {
    Float_t _t = m_costData->getSeqAlgTimeStart(_e, _f);

    if (isZero(_t) == kTRUE) { // Annoyingly - if alg was cached it will have a zero, need to look back for the last alg
                               // to actually run
      _t = Config::config().getFloat(kEventElapsed);
    } else {
      Config::config().setFloat(kEventElapsed, _t, "EventElapsed", kUnlocked);
    }
    // Were we first?
    Float_t _eventStart = Config::config().getFloat(kEventStartTime);
    if (isZero(_eventStart) == kTRUE) {
      _eventStart = _t;
      Config::config().setFloat(kEventStartTime, _eventStart, "EventStart", kUnlocked);
    }
    Float_t _elapsed = _t - _eventStart;
    if (_elapsed < 0) _elapsed += 3600.;
    return _elapsed;
  }

  /**
   * If this counter is being used by the FullEvent Monitor and hence being called exactly once for a single algorithm
   *execution.
   * Then we save additional data and add it as decorations to this counter, these will be retrieved by the table
   *plotting code
   * @param _e Sequence index in D3PD.
   * @param _f Location of algorithm within parent sequence.
   * @param _weight Event weight.
   */
  void CounterAlgorithm::fullExecutionInformation(UInt_t _e, UInt_t _f, Float_t _weight) {
    UNUSED(_weight);

    decorate(kDecStartTime, m_costData->getSeqAlgTimeStart(_e, _f));
    decorate(kDecElapsedTime, getElapsedTime(_e, _f));
    decorate(kDecCallOrCache, m_costData->getSeqAlgIsCalled(_e, _f) ? Config::config().getStr(
               kCalledString) : Config::config().getStr(kCachedString));

    //RoI Info
    std::string _roiString = "";
    if (m_costData->getSeqAlgNRoI(_e, _f) == 0) {
      _roiString = "No RoI";
    } else {
      for (Int_t _i = 0; _i < m_costData->getSeqAlgNRoI(_e, _f); ++_i) {
        if (_i != 0) _roiString += " | ";
        Int_t _roiLocation = m_costData->getSeqAlgRoILocation(_e, _f, _i);
        _roiString += m_costData->getRoITypeString(_roiLocation);
        if (_roiLocation != -1 && m_costData->getRoIEta(_roiLocation) > -9000) { // Some ROIs do not have a location -
                                                                                 // these return -9999 for these vars
          _roiString += std::string(" (&eta;");
          _roiString += floatToString(m_costData->getRoIEta(_roiLocation), 1);
          _roiString += std::string(" - &phi;");
          _roiString += floatToString(m_costData->getRoIPhi(_roiLocation), 1);
          _roiString += std::string(")");
        }
      }
    }
    decorate(kDecROIString, _roiString);

    // Which ROBs did we access?
    std::map<std::string, UInt_t> _ROSReqs;
    std::set<Int_t> _ros = m_costData->getSeqAlgROBLocations(_e, _f);
    for (std::set<Int_t>::iterator _robIt = _ros.begin(); _robIt != _ros.end(); ++_robIt) {
      Int_t _rob = (*_robIt);
      if (m_costData->getROBDataN(_rob) == 0) continue;
      Int_t _RobId = m_costData->getROBDataID(_rob, 0);
      const std::string _RobinName = ROSConfService::rosConfService().getRobinNameFromId((UInt_t) _RobId);
      const std::string _RobName = ROSConfService::rosConfService().getRosNameFromFromRobinName(_RobinName);
      _ROSReqs[ _RobName ] += 1;
    }

    std::string _rosString = "";
    for (std::map<std::string, UInt_t>::iterator _it = _ROSReqs.begin(); _it != _ROSReqs.end(); ++_it) {
      if (_it != _ROSReqs.begin()) _rosString += " | ";
      _rosString += _it->first;
      _rosString += " x";
      _rosString += intToString(_it->second);
    }
    decorate(kDecROSString, _rosString);
  }

  /**
   * Perform end-of-event monitoring for this algorithm.
   * What was determined to be the first execution's time is saved, the total time for the event is used to calculate
   *per-event averages.
   */
  void CounterAlgorithm::endEvent(Float_t _weight) {
    if (m_firstAlgStartTime != FLT_MAX) {
      //m_eventWeight = 1. - m_eventWeight;
      m_dataStore.store(kVarEventsActive, 1., /*m_eventWeight * */ _weight);
      m_dataStore.store(kVarFirstTime, m_firstAlgTime, _weight);
      m_firstAlgStartTime = FLT_MAX; // Reset first-exec time
      m_firstAlgTime = 0.;
      //m_firstAlgTimeWeight = 0.;
      //m_eventWeight = 1.;
      //m_chainsSeen.clear();
    }

    m_dataStore.setVariableDenominator(kVarTime, s_eventTimeExecute);
    m_dataStore.endEvent();
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterAlgorithm::debug(UInt_t _e, UInt_t _a) {
    Info("CounterAlgorithm::debug", "Algorithm Name:%s Class:%s ID:%i Pos:%i isCached:%i "
                                    "isCalled:%i NRoI:%i Timer:%.2f",
         m_name.c_str(),
         getStrDecoration(kDecAlgClassName).c_str(),
         getID(),
         m_costData->getSeqAlgPosition(_e, _a),
         m_costData->getSeqAlgIsCached(_e, _a),
         m_costData->getSeqAlgIsCalled(_e, _a),
         m_costData->getSeqAlgNRoI(_e, _a),
         //m_costData->getSeqAlgRoIIndexFirst(_e, _a), //TODO - DISPLAY ALG INFO
         m_costData->getSeqAlgTimer(_e, _a)
         );
  }
} // namespace TrigCostRootAnalysis
