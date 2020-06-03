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
   * @param name Const ref to algorithm's name
   * @param ID Algorithms ID number. As an algorithm has two ID numbers, we take a hash of its name as its ID.
   */
  CounterAlgorithm::CounterAlgorithm(const TrigCostData* costData, const std::string& name, Int_t ID,
                                     UInt_t detailLevel, MonitorBase* parent) :
    CounterBase(costData, name, ID, detailLevel, parent),
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
   * @param e Sequence index in D3PD.
   * @param f Location of algorithm within parent sequence.
   * @param weight Event weight.
   */
  void CounterAlgorithm::processEventCounter(UInt_t e, UInt_t f, Float_t weight) {
    ++m_calls;

    // After some thought - this just doesn't work
    //Float_t prescaleFactor = getPrescaleFactor(e);
    //weight *= prescaleFactor;

    // Special case. If we are running as part of the FullEvent monitor, we should keep weight=1 to make the output
    // readable
    if (m_detailLevel == 0 && m_calls == 1) weight = 1.;

    if (isZero(weight) == kTRUE) {
      return;
    }

    if (Config::config().debug()) debug(e, f);

    // After some thought - this just doesn't work
    // const std::string myChain = TrigConfInterface::getHLTNameFromChainID(
    // m_costData->getSequenceChannelCounter(e));
    // if ( m_chainsSeen.count( myChain ) == 0) {
    //   m_eventWeight *= (1. - prescaleFactor);
    //   m_chainsSeen.insert( myChain );
    // }

    m_dataStore.store(kVarCalls, 1., weight);

    m_dataStore.store(kVarAlgCalls, m_costData->getSeqAlgIsCalled(e, f), weight);
    m_dataStore.store(kVarAlgCaches, m_costData->getSeqAlgIsCached(e, f), weight);

    m_dataStore.store(kVarROSCalls, m_costData->getSeqAlgROSCalls(e, f), weight);
    if (m_costData->getSeqAlgROBRequests(e, f) != 0) {
      m_dataStore.store(kVarROBReqs, m_costData->getSeqAlgROBRequests(e, f), weight);
      m_dataStore.store(kVarROBReqSize, m_costData->getSeqAlgROBRequestSize(e, f), weight);
    }
    if (m_costData->getSeqAlgROBRetrievals(e, f) != 0) {
      m_dataStore.store(kVarROBRets, m_costData->getSeqAlgROBRetrievals(e, f), weight);
      m_dataStore.store(kVarROBRetSize, m_costData->getSeqAlgROBRetrievalSize(e, f), weight);
    }
    if (m_costData->getSeqAlgROBOthers(e, f) != 0) {
      m_dataStore.store(kVarROBOther, m_costData->getSeqAlgROBOthers(e, f), weight);
    }

    if (m_costData->getSeqAlgTimer(e, f) > Config::config().getInt(kSlowThreshold)) {
      m_dataStore.store(kVarCallsSlow, 1., weight);
    }

    // We record separately the the time of the first call in the event
    if (m_costData->getSeqAlgTimeStart(e, f) < m_firstAlgStartTime) {
      m_firstAlgStartTime = m_costData->getSeqAlgTimeStart(e, f);
      m_firstAlgTime = m_costData->getSeqAlgTimer(e, f);
      //m_firstAlgTimeWeight = weight;
    }

    m_dataStore.store(kVarTime, m_costData->getSeqAlgTimer(e, f), weight);

    // Here we only do the timing if the alg was callled NOT cached
    if (m_costData->getSeqAlgIsCalled(e, f) == 1) m_dataStore.store(kVarTimeExec, m_costData->getSeqAlgTimer(e,
                                                                                                               f),
                                                                      weight);

    Float_t ROSTime = m_costData->getSeqAlgROSTime(e, f);
    if (isZero(ROSTime) == kFALSE) {
      m_dataStore.store(kVarROSTime, m_costData->getSeqAlgROSTime(e, f), weight);
      m_dataStore.store(kVarCPUTime, m_costData->getSeqAlgTimer(e, f) - ROSTime, weight);
    }
    // Gather time statistics for this algorithm call
    s_eventTimeExecute += m_costData->getSeqAlgTimer(e, f) * weight;

    // If calling this counter exactly once on a alg single execution - save some extra levels details.
    // This allows for a full-scale debug of a single event execution.
    if (m_detailLevel == 0 && m_calls == 1) fullExecutionInformation(e, f, weight);
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * For an algorithm, we need to figure out the chain which the alg belongs to and then check it's enabled and then
   * scale by its L1 prescale
   * @return Multiplicative weighting factor
   */
  Double_t CounterAlgorithm::getPrescaleFactor(UInt_t e) {
    return TrigXMLService::trigXMLService().getHLTCostWeightingFactor(
      TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(e),
                                               m_costData->getSequenceLevel(e)));
  }

  /**
   * @param e Sequence index in D3PD.
   * @param f Location of algorithm within parent sequence
   * @return How much time has passed in seconds from the beginning of the event to this alg's execution
   */
  Float_t CounterAlgorithm::getElapsedTime(UInt_t e, UInt_t f) {
    Float_t t = m_costData->getSeqAlgTimeStart(e, f);

    if (isZero(t) == kTRUE) { // Annoyingly - if alg was cached it will have a zero, need to look back for the last alg
                               // to actually run
      t = Config::config().getFloat(kEventElapsed);
    } else {
      Config::config().setFloat(kEventElapsed, t, "EventElapsed", kUnlocked);
    }
    // Were we first?
    Float_t eventStart = Config::config().getFloat(kEventStartTime);
    if (isZero(eventStart) == kTRUE) {
      eventStart = t;
      Config::config().setFloat(kEventStartTime, eventStart, "EventStart", kUnlocked);
    }
    Float_t elapsed = t - eventStart;
    if (elapsed < 0) elapsed += 3600.;
    return elapsed;
  }

  /**
   * If this counter is being used by the FullEvent Monitor and hence being called exactly once for a single algorithm
   *execution.
   * Then we save additional data and add it as decorations to this counter, these will be retrieved by the table
   *plotting code
   * @param e Sequence index in D3PD.
   * @param f Location of algorithm within parent sequence.
   * @param weight Event weight.
   */
  void CounterAlgorithm::fullExecutionInformation(UInt_t e, UInt_t f, Float_t /*weight*/) {

    decorate(kDecStartTime, m_costData->getSeqAlgTimeStart(e, f));
    decorate(kDecElapsedTime, getElapsedTime(e, f));
    decorate(kDecCallOrCache, m_costData->getSeqAlgIsCalled(e, f) ? Config::config().getStr(
               kCalledString) : Config::config().getStr(kCachedString));

    //RoI Info
    std::string roiString = "";
    if (m_costData->getSeqAlgNRoI(e, f) == 0) {
      roiString = "No RoI";
    } else {
      for (Int_t i = 0; i < m_costData->getSeqAlgNRoI(e, f); ++i) {
        if (i != 0) roiString += " | ";
        Int_t roiLocation = m_costData->getSeqAlgRoILocation(e, f, i);
        roiString += m_costData->getRoITypeString(roiLocation);
        if (roiLocation != -1 && m_costData->getRoIEta(roiLocation) > -9000) { // Some ROIs do not have a location -
                                                                                 // these return -9999 for these vars
          roiString += std::string(" (&eta;");
          roiString += floatToString(m_costData->getRoIEta(roiLocation), 1);
          roiString += std::string(" - &phi;");
          roiString += floatToString(m_costData->getRoIPhi(roiLocation), 1);
          roiString += std::string(")");
        }
      }
    }
    decorate(kDecROIString, roiString);

    // Which ROBs did we access?
    std::map<std::string, UInt_t> ROSReqs;
    std::set<Int_t> ros = m_costData->getSeqAlgROBLocations(e, f);
    for (std::set<Int_t>::iterator robIt = ros.begin(); robIt != ros.end(); ++robIt) {
      Int_t rob = (*robIt);
      if (m_costData->getROBDataN(rob) == 0) continue;
      Int_t RobId = m_costData->getROBDataID(rob, 0);
      const std::string RobinName = ROSConfService::rosConfService().getRobinNameFromId((UInt_t) RobId);
      const std::string RobName = ROSConfService::rosConfService().getRosNameFromFromRobinName(RobinName);
      ROSReqs[ RobName ] += 1;
    }

    std::string rosString = "";
    for (std::map<std::string, UInt_t>::iterator it = ROSReqs.begin(); it != ROSReqs.end(); ++it) {
      if (it != ROSReqs.begin()) rosString += " | ";
      rosString += it->first;
      rosString += " x";
      rosString += intToString(it->second);
    }
    decorate(kDecROSString, rosString);
  }

  /**
   * Perform end-of-event monitoring for this algorithm.
   * What was determined to be the first execution's time is saved, the total time for the event is used to calculate
   *per-event averages.
   */
  void CounterAlgorithm::endEvent(Float_t weight) {
    if (m_firstAlgStartTime != FLT_MAX) {
      //m_eventWeight = 1. - m_eventWeight;
      m_dataStore.store(kVarEventsActive, 1., /*m_eventWeight * */ weight);
      m_dataStore.store(kVarFirstTime, m_firstAlgTime, weight);
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
  void CounterAlgorithm::debug(UInt_t e, UInt_t a) {
    Info("CounterAlgorithm::debug", "Algorithm Name:%s Class:%s ID:%i Pos:%i isCached:%i "
                                    "isCalled:%i NRoI:%i Timer:%.2f",
         m_name.c_str(),
         getStrDecoration(kDecAlgClassName).c_str(),
         getID(),
         m_costData->getSeqAlgPosition(e, a),
         m_costData->getSeqAlgIsCached(e, a),
         m_costData->getSeqAlgIsCalled(e, a),
         m_costData->getSeqAlgNRoI(e, a),
         //m_costData->getSeqAlgRoIIndexFirst(e, a), //TODO - DISPLAY ALG INFO
         m_costData->getSeqAlgTimer(e, a)
         );
  }
} // namespace TrigCostRootAnalysis
