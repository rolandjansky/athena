// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <assert.h>

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterChain.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {
  Float_t CounterChain::s_eventTimeExecute = 0.; //Static timer for whole event

  /**
   * Chain counter constructor. Sets values of internal variables and sets up data store.
   * @param name Const ref to chain's name
   * @param ID Chain's HLT ID number.
   */
  CounterChain::CounterChain(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel,
                             MonitorBase* parent)
    : CounterBase(costData, name, ID, detailLevel, parent),
    m_prescaleWeight(1.) {
    // Register variables to study.
    if (m_detailLevel == 0) {
      m_dataStore.setHistogramming(kFALSE);
    }

    m_dataStore.newVariable(kVarEventsActive).setSavePerEvent();
    m_dataStore.newVariable(kVarEventsPassed).setSavePerEvent();
    //m_dataStore.newVariable(kVarEventsPassedNoPS).setSavePerEvent(); // Not used
    //m_dataStore.newVariable(kVarEventsPassthrough).setSavePerEvent(); // Not used
    m_dataStore.newVariable(kVarEventsSlow).setSavePerEvent();
    //m_dataStore.newVariable(kVarTotalPrescale).setSavePerEvent(); // Not used
    m_dataStore.newVariable(kVarAlgCalls)
     .setSavePerEvent("Number Of Algorithm Executions Per Event;Algorithm Executions;Events");
    m_dataStore.newVariable(kVarAlgCaches)
     .setSavePerEvent("Number Of Cached Algorithms Per Event;Cached Algorithims;Events");
    m_dataStore.newVariable(kVarTime)
     .setSavePerEvent("Chain Total Time Per Event;Chain Total Time per Event [ms];Events")
     .setSavePerEventFraction("Fractional Chain Total Time;Chain Total Time/Event Total Time;Events");
    m_dataStore.newVariable(kVarRerunTime).setSavePerEvent();
    m_dataStore.newVariable(kVarROBReqs)
     .setSavePerEvent("Number Of Cached ROBs Per Event;Cached ROBs;Events");
    m_dataStore.newVariable(kVarROBRets)
     .setSavePerEvent("Number Of Retrieved ROBs Per Event;Retrieved ROBs;Events");
    m_dataStore.newVariable(kVarROBReqSize)
     .setSavePerEvent("Size Of Cached ROBs Per Event;Cached ROBs Size [kB];Events");
    m_dataStore.newVariable(kVarROBRetSize)
     .setSavePerEvent("Size Of Retrieved ROBs Per Event;Retrieved ROBs Size [kB];Events");

    if (getName() == Config::config().getStr(kDummyString)) return;

    // Get my prescale
    m_prescaleWeight = TrigXMLService::trigXMLService().getHLTCostWeightingFactor(getName());
    decorate(kEffectivePrescale, m_prescaleWeight);
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterChain::~CounterChain() {
  }

  /**
   * Reset per-event counter(s). This is a static so this needs to only be called on one instance of this class,
   * not all instances.
   */
  void CounterChain::startEvent() {
    s_eventTimeExecute = 0.; // Reset event chain timer (static)
  }

  /**
   * Perform monitoring of a chain within an event.
   * @param e Chain index in D3PD.
   * @param f Unused by this counter.
   * @param weight Event weight.
   */
  void CounterChain::processEventCounter(UInt_t e, UInt_t /*f*/, Float_t weight) {
    ++m_calls;

    if (Config::config().debug()) debug(e);

    weight *= getPrescaleFactor();
    if (isZero(weight) == kTRUE) return;

    // Increase total time
    Float_t chainTime = m_costData->getChainTimerFromSequences(e);

    //m_dataStore.store(kVarTotalPrescale, TrigConfInterface::getPrescale( m_name ), weight);
    m_dataStore.store(kVarEventsActive, 1., weight);
    m_dataStore.store(kVarEventsPassed, (Int_t) m_costData->getIsChainPassed(e), weight);
    m_dataStore.store(kVarTime, chainTime, weight);

    if (m_costData->getIsChainResurrected(e)) {
      m_dataStore.store(kVarRerunTime, chainTime, weight);
    }

    s_eventTimeExecute += chainTime * weight; // Tabulate over all chains in event

    if (chainTime > Config::config().getInt(kSlowThreshold)) {
      m_dataStore.store(kVarEventsSlow, 1., weight);
    }

    m_dataStore.store(kVarAlgCalls, m_costData->getChainAlgCalls(e), weight);
    m_dataStore.store(kVarAlgCaches, m_costData->getChainAlgCaches(e), weight);

    if (m_costData->getChainROBRetrievals(e) != 0) {
      m_dataStore.store(kVarROBRets, m_costData->getChainROBRetrievals(e), weight);
      m_dataStore.store(kVarROBRetSize, m_costData->getChainROBRetrievalSize(e), weight);
    }
    if (m_costData->getChainROBRequests(e) != 0) {
      m_dataStore.store(kVarROBReqs, m_costData->getChainROBRequests(e), weight);
      m_dataStore.store(kVarROBReqSize, m_costData->getChainROBRequestSize(e), weight);
    }

    // We should have as many algs here as we recorded as "called" and "cached"
    assert(m_costData->getChainAlgs(e).size() == m_costData->getChainAlgCalls(e) + m_costData->getChainAlgCaches(e));
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * For a chain, we always scale by 1/L1PS. The HLT PS is applied separatly and only to the ChainsPassed item.
   * This computes the rate for the chain for this simple case.
   * Note that this is 0 if either level has PS < 0
   * @return Multiplicitive weighting factor
   */
  Double_t CounterChain::getPrescaleFactor(UInt_t /*e*/) {
    return m_prescaleWeight;
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterChain::endEvent(Float_t /*weight*/) {
    m_dataStore.setVariableDenominator(kVarTime, s_eventTimeExecute);
    m_dataStore.endEvent();
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterChain::debug(UInt_t e) {
    Info("CounterChain::debug", "Counter Name:%s ID:%i Event:%i ChainTime:%.2f ChainTimeFromSeq:%.2f "
                                "PS:%.2f LB:%i Pass:%i PassRaw:%i PT:%i SeqCalls:%i AlgCall:%i AlgCache:%i",
         m_name.c_str(),
         getID(),
         m_costData->getEventNumber(),
         m_costData->getChainTimer(e),
         m_costData->getChainTimerFromSequences(e),
         TrigConfInterface::getPrescale(m_name),
         m_costData->getLumi(),
         m_costData->getIsChainPassed(e),
         m_costData->getIsChainPassedRaw(e),
         m_costData->getIsChainPassthrough(e),
         m_costData->getChainSeqCalls(e),
         m_costData->getChainAlgCalls(e),
         m_costData->getChainAlgCaches(e)
         );
  }
} // namespace TrigCostRootAnalysis
