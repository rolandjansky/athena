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
   * @param _name Const ref to chain's name
   * @param _ID Chain's HLT ID number.
   */
  CounterChain::CounterChain( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel, MonitorBase* _parent )
    : CounterBase(_costData, _name, _ID, _detailLevel, _parent),
    m_prescaleWeight(1.) {

    // Register variables to study.
    if (m_detailLevel == 0) {
      m_dataStore.setHistogramming( kFALSE );
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
    m_prescaleWeight = TrigXMLService::trigXMLService().getHLTCostWeightingFactor( getName() );
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
   * @param _e Chain index in D3PD.
   * @param _f Unused by this counter.
   * @param _weight Event weight.
   */
  void CounterChain::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;
    UNUSED( _f );

    if ( Config::config().debug() ) debug(_e);

    _weight *= getPrescaleFactor();
    if (isZero(_weight) == kTRUE) return;

    // Increase total time
    Float_t _chainTime = m_costData->getChainTimerFromSequences(_e);

    //m_dataStore.store(kVarTotalPrescale, TrigConfInterface::getPrescale( m_name ), _weight);
    m_dataStore.store(kVarEventsActive, 1., _weight);
    m_dataStore.store(kVarEventsPassed, (Int_t) m_costData->getIsChainPassed(_e), _weight);
    m_dataStore.store(kVarTime, _chainTime, _weight);

    if (m_costData->getIsChainResurrected(_e)) {
      m_dataStore.store(kVarRerunTime, _chainTime, _weight);
    }

    s_eventTimeExecute += _chainTime * _weight; // Tabulate over all chains in event

    if ( _chainTime > Config::config().getInt(kSlowThreshold) ) {
      m_dataStore.store(kVarEventsSlow, 1., _weight);
    }

    m_dataStore.store(kVarAlgCalls, m_costData->getChainAlgCalls(_e), _weight);
    m_dataStore.store(kVarAlgCaches, m_costData->getChainAlgCaches(_e), _weight);

    if ( m_costData->getChainROBRetrievals(_e) != 0 ) {
      m_dataStore.store(kVarROBRets, m_costData->getChainROBRetrievals(_e), _weight);
      m_dataStore.store(kVarROBRetSize, m_costData->getChainROBRetrievalSize(_e), _weight);
    }
    if ( m_costData->getChainROBRequests(_e) != 0) {
      m_dataStore.store(kVarROBReqs, m_costData->getChainROBRequests(_e), _weight);
      m_dataStore.store(kVarROBReqSize, m_costData->getChainROBRequestSize(_e), _weight);
    }

    // We should have as many algs here as we recorded as "called" and "cached"
    assert( m_costData->getChainAlgs(_e).size() == m_costData->getChainAlgCalls(_e) + m_costData->getChainAlgCaches(_e));

  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * For a chain, we always scale by 1/L1PS. The HLT PS is applied separatly and only to the ChainsPassed item.
   * This computes the rate for the chain for this simple case.
   * Note that this is 0 if either level has PS < 0
   * @return Multiplicitive weighting factor
   */
  Double_t CounterChain::getPrescaleFactor(UInt_t _e) {
    UNUSED(_e);
    return m_prescaleWeight;
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterChain::endEvent(Float_t _weight) {
    UNUSED(_weight);
    m_dataStore.setVariableDenominator(kVarTime, s_eventTimeExecute);
    m_dataStore.endEvent();

  }

  /**
   * Output debug information on this call to the console
   */
  void CounterChain::debug(UInt_t _e) {
    Info("CounterChain::debug", "Counter Name:%s ID:%i Event:%i ChainTime:%.2f ChainTimeFromSeq:%.2f "
         "PS:%.2f LB:%i Pass:%i PassRaw:%i PT:%i SeqCalls:%i AlgCall:%i AlgCache:%i",
         m_name.c_str(),
         getID(),
         m_costData->getEventNumber(),
         m_costData->getChainTimer(_e),
         m_costData->getChainTimerFromSequences(_e),
         TrigConfInterface::getPrescale( m_name ),
         m_costData->getLumi(),
         m_costData->getIsChainPassed(_e),
         m_costData->getIsChainPassedRaw(_e),
         m_costData->getIsChainPassthrough(_e),
         m_costData->getChainSeqCalls(_e),
         m_costData->getChainAlgCalls(_e),
         m_costData->getChainAlgCaches(_e)
        );
  }


} // namespace TrigCostRootAnalysis
