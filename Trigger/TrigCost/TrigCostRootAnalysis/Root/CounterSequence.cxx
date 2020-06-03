// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterSequence.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

namespace TrigCostRootAnalysis {
  Float_t CounterSequence::s_eventTimeExecute = 0.;

  /**
   * Sequence counter constructor. Sets values of internal variables and sets up data store.
   * @param name Const ref to sequence's name
   * @param ID Sequence's index number.
   */
  CounterSequence::CounterSequence(const TrigCostData* costData, const std::string& name, Int_t ID,
                                   UInt_t detailLevel, MonitorBase* parent)
    : CounterBase(costData, name, ID, detailLevel, parent),
    m_eventWeight(1.) {
    // Reg. variables to study.
    m_dataStore.newVariable(kVarEventsActive).setSavePerEvent();
    m_dataStore.newVariable(kVarCalls).setSavePerEvent();
    m_dataStore.newVariable(kVarCallsSlow).setSavePerEvent();

    // Time
    m_dataStore.newVariable(kVarTime)
     .setSavePerCall("Sequence Total Time Per Call;Sequence Total Time per Call [ms];Calls")
     .setSavePerEvent("Sequence Total Time Per Event;Sequence Total Time per Event [ms];Events")
     .setSavePerEventFraction("Fractional Sequence Total Time;Sequence Total Time/Event Total Time;Events");
    m_dataStore.newVariable(kVarCPUTime)
     .setSavePerCall("Sequence CPU Time Per Call;Sequence CPU Time [ms];Calls")
     .setSavePerEvent("Sequence CPU Time Per Event;Sequence CPU Time [ms];Events");
    m_dataStore.newVariable(kVarROSTime)
     .setSavePerCall("Sequence ROB Time Per Call;Sequence ROB Time [ms];Calls")
     .setSavePerEvent("Sequence ROB Time Per Event;Sequence ROB Time [ms];Events");

    // Cache status
    m_dataStore.newVariable(kVarAlgCalls)
     .setSavePerCall("Number Of Alg Executions Per Sequence Call;Alg Executions per Sequence Call;Calls")
     .setSavePerEvent("Number Of Alg Executions Per Event;Alg Executions per Event;Events");
    m_dataStore.newVariable(kVarAlgCaches)
     .setSavePerCall("Number Of Cached Algs Per Sequence Call;Cached Algs per Sequence Call;Calls")
     .setSavePerEvent("Number Of Cached Algs Per Event;Cached Algs per Event;Events");

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

    m_dataStore.newVariable(kVarROBOther).setSavePerCall().setSavePerEvent();
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterSequence::~CounterSequence( ) {
  }

  /**
   * Reset per-event counter(s). This is a static so this needs to only be called on one instance of this class,
   * not all instances.
   */
  void CounterSequence::startEvent() {
    s_eventTimeExecute = 0.;
  }

  /**
   * Perform monitoring of a sequence within an event.
   * @param e Sequence index in D3PD.
   * @param f Unused by this counter.
   * @param weight Event weight.
   */
  void CounterSequence::processEventCounter(UInt_t e, UInt_t /*f*/, Float_t weight) {
    ++m_calls;

    Float_t prescaleFactor = getPrescaleFactor(e);
    weight *= prescaleFactor;
    if (isZero(weight) == kTRUE) return;

    if (Config::config().debug()) debug(e);

    const std::string myChain = TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(e));
    if (m_chainsSeen.count(myChain) == 0) {
      m_eventWeight *= (1. - prescaleFactor);
      m_chainsSeen.insert(myChain);
    }

    m_dataStore.store(kVarCalls, 1., weight);

    // Get time for sequence
    Float_t rosTime = m_costData->getSeqROSTime(e);
    Float_t sequenceTime = m_costData->getSequenceTime(e);
    //if ( isZero(sequenceTime) == kTRUE ) {
    sequenceTime = m_costData->getSequenceAlgTotalTime(e);
    //}
    s_eventTimeExecute += sequenceTime * weight; // Add to static variable to keep track during event
    m_dataStore.store(kVarTime, sequenceTime, weight);
    if (isZero(rosTime) == kFALSE) {
      m_dataStore.store(kVarROSTime, rosTime, weight);
      m_dataStore.store(kVarCPUTime, sequenceTime - rosTime, weight);
    }

    if (sequenceTime > Config::config().getInt(kSlowThreshold)) {
      m_dataStore.store(kVarCallsSlow, 1., weight);
    }

    // Get alg cache staus
    m_dataStore.store(kVarAlgCalls, m_costData->getSequenceAlgCalls(e), weight);
    m_dataStore.store(kVarAlgCaches, m_costData->getSequenceAlgCaches(e), weight);

    // Get alg ROB status
    m_dataStore.store(kVarROSCalls, m_costData->getSeqROSCalls(e), weight);
    if (m_costData->getSeqROBRequests(e) != 0) {
      m_dataStore.store(kVarROBReqs, m_costData->getSeqROBRequests(e), weight);
      m_dataStore.store(kVarROBReqSize, m_costData->getSeqROBRequestSize(e), weight);
    }
    if (m_costData->getSeqROBRetrievals(e) != 0) {
      m_dataStore.store(kVarROBRets, m_costData->getSeqROBRetrievals(e), weight);
      m_dataStore.store(kVarROBRetSize, m_costData->getSeqROBRetrievalSize(e), weight);
    }
    if (m_costData->getSeqROBOthers(e) != 0) {
      m_dataStore.store(kVarROBOther, m_costData->getSeqROBOthers(e), weight);
    }
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * For a sequence, we need to figure out the chain which the sequence belongs to and then check it's enabled and then
   * scale by its L1 prescale
   * @return Multiplicative weighting factor
   */
  Double_t CounterSequence::getPrescaleFactor(UInt_t e) {
    return TrigXMLService::trigXMLService().getHLTCostWeightingFactor(
      TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(e),
                                               m_costData->getSequenceLevel(e)));
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterSequence::endEvent(Float_t weight) {
    if (m_chainsSeen.size() > 0) {
      m_eventWeight = 1. - m_eventWeight;
      m_dataStore.store(kVarEventsActive, 1., m_eventWeight * weight);
      m_eventWeight = 1.;
      m_chainsSeen.clear();
    }

    m_dataStore.setVariableDenominator(kVarTime, s_eventTimeExecute);
    m_dataStore.endEvent();
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterSequence::debug(UInt_t e) {
    Info("CounterSequence::debug", "Seq Name:%s ID:%i Evnt:%i Lvl:%i Chnl:%i Index:%i SeqTimer:%.2f SeqAlgTimer:%.2f"
                                   " isAlreadyExec:%i isExec:%i isInitial:%i isPrev:%i nAlgs:%i nAlgCalls:%i nAlgCaches:%i"
                                   " ROSCall:%i ROSTime:%.2f ROBRet:%i ROBRetSize:%.2f ROBReq:%i ROBReqSize:%.2f ROBOther:%i",
         m_name.c_str(),
         getID(),
         m_costData->getEventNumber(),
         m_costData->getSequenceLevel(e),
         m_costData->getSequenceChannelCounter(e),
         m_costData->getSequenceIndex(e),
         m_costData->getSequenceTime(e),
         m_costData->getSequenceAlgTotalTime(e),
         (Int_t) m_costData->getIsSequenceAlreadyExecuted(e),
         (Int_t) m_costData->getIsSequenceExecuted(e),
         (Int_t) m_costData->getIsSequenceInitial(e),
         (Int_t) m_costData->getIsSequencePrevious(e),
         m_costData->getNSeqAlgs(e),
         m_costData->getSequenceAlgCalls(e),
         m_costData->getSequenceAlgCaches(e),
         m_costData->getSeqROSCalls(e),
         m_costData->getSeqROSTime(e),
         m_costData->getSeqROBRetrievals(e),
         m_costData->getSeqROBRetrievalSize(e),
         m_costData->getSeqROBRequests(e),
         m_costData->getSeqROBRequestSize(e),
         m_costData->getSeqROBOthers(e));
  }
} // namespace TrigCostRootAnalysis
