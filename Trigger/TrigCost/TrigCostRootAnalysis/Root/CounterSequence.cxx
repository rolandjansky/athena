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
   * @param _name Const ref to sequence's name
   * @param _ID Sequence's index number.
   */
  CounterSequence::CounterSequence(const TrigCostData* _costData, const std::string& _name, Int_t _ID,
                                   UInt_t _detailLevel, MonitorBase* _parent)
    : CounterBase(_costData, _name, _ID, _detailLevel, _parent),
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
   * @param _e Sequence index in D3PD.
   * @param _f Unused by this counter.
   * @param _weight Event weight.
   */
  void CounterSequence::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;
    UNUSED(_f);

    Float_t _prescaleFactor = getPrescaleFactor(_e);
    _weight *= _prescaleFactor;
    if (isZero(_weight) == kTRUE) return;

    if (Config::config().debug()) debug(_e);

    const std::string _myChain = TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(_e));
    if (m_chainsSeen.count(_myChain) == 0) {
      m_eventWeight *= (1. - _prescaleFactor);
      m_chainsSeen.insert(_myChain);
    }

    m_dataStore.store(kVarCalls, 1., _weight);

    // Get time for sequence
    Float_t _rosTime = m_costData->getSeqROSTime(_e);
    Float_t _sequenceTime = m_costData->getSequenceTime(_e);
    //if ( isZero(_sequenceTime) == kTRUE ) {
    _sequenceTime = m_costData->getSequenceAlgTotalTime(_e);
    //}
    s_eventTimeExecute += _sequenceTime * _weight; // Add to static variable to keep track during event
    m_dataStore.store(kVarTime, _sequenceTime, _weight);
    if (isZero(_rosTime) == kFALSE) {
      m_dataStore.store(kVarROSTime, _rosTime, _weight);
      m_dataStore.store(kVarCPUTime, _sequenceTime - _rosTime, _weight);
    }

    if (_sequenceTime > Config::config().getInt(kSlowThreshold)) {
      m_dataStore.store(kVarCallsSlow, 1., _weight);
    }

    // Get alg cache staus
    m_dataStore.store(kVarAlgCalls, m_costData->getSequenceAlgCalls(_e), _weight);
    m_dataStore.store(kVarAlgCaches, m_costData->getSequenceAlgCaches(_e), _weight);

    // Get alg ROB status
    m_dataStore.store(kVarROSCalls, m_costData->getSeqROSCalls(_e), _weight);
    if (m_costData->getSeqROBRequests(_e) != 0) {
      m_dataStore.store(kVarROBReqs, m_costData->getSeqROBRequests(_e), _weight);
      m_dataStore.store(kVarROBReqSize, m_costData->getSeqROBRequestSize(_e), _weight);
    }
    if (m_costData->getSeqROBRetrievals(_e) != 0) {
      m_dataStore.store(kVarROBRets, m_costData->getSeqROBRetrievals(_e), _weight);
      m_dataStore.store(kVarROBRetSize, m_costData->getSeqROBRetrievalSize(_e), _weight);
    }
    if (m_costData->getSeqROBOthers(_e) != 0) {
      m_dataStore.store(kVarROBOther, m_costData->getSeqROBOthers(_e), _weight);
    }
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * For a sequence, we need to figure out the chain which the sequence belongs to and then check it's enabled and then
   * scale by its L1 prescale
   * @return Multiplicative weighting factor
   */
  Double_t CounterSequence::getPrescaleFactor(UInt_t _e) {
    return TrigXMLService::trigXMLService().getHLTCostWeightingFactor(
      TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(_e),
                                               m_costData->getSequenceLevel(_e)));
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterSequence::endEvent(Float_t _weight) {
    if (m_chainsSeen.size() > 0) {
      m_eventWeight = 1. - m_eventWeight;
      m_dataStore.store(kVarEventsActive, 1., m_eventWeight * _weight);
      m_eventWeight = 1.;
      m_chainsSeen.clear();
    }

    m_dataStore.setVariableDenominator(kVarTime, s_eventTimeExecute);
    m_dataStore.endEvent();
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterSequence::debug(UInt_t _e) {
    Info("CounterSequence::debug", "Seq Name:%s ID:%i Evnt:%i Lvl:%i Chnl:%i Index:%i SeqTimer:%.2f SeqAlgTimer:%.2f"
                                   " isAlreadyExec:%i isExec:%i isInitial:%i isPrev:%i nAlgs:%i nAlgCalls:%i nAlgCaches:%i"
                                   " ROSCall:%i ROSTime:%.2f ROBRet:%i ROBRetSize:%.2f ROBReq:%i ROBReqSize:%.2f ROBOther:%i",
         m_name.c_str(),
         getID(),
         m_costData->getEventNumber(),
         m_costData->getSequenceLevel(_e),
         m_costData->getSequenceChannelCounter(_e),
         m_costData->getSequenceIndex(_e),
         m_costData->getSequenceTime(_e),
         m_costData->getSequenceAlgTotalTime(_e),
         (Int_t) m_costData->getIsSequenceAlreadyExecuted(_e),
         (Int_t) m_costData->getIsSequenceExecuted(_e),
         (Int_t) m_costData->getIsSequenceInitial(_e),
         (Int_t) m_costData->getIsSequencePrevious(_e),
         m_costData->getNSeqAlgs(_e),
         m_costData->getSequenceAlgCalls(_e),
         m_costData->getSequenceAlgCaches(_e),
         m_costData->getSeqROSCalls(_e),
         m_costData->getSeqROSTime(_e),
         m_costData->getSeqROBRetrievals(_e),
         m_costData->getSeqROBRetrievalSize(_e),
         m_costData->getSeqROBRequests(_e),
         m_costData->getSeqROBRequestSize(_e),
         m_costData->getSeqROBOthers(_e));
  }
} // namespace TrigCostRootAnalysis
