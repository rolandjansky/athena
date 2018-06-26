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
#include "../TrigCostRootAnalysis/CounterSliceCPU.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {
  /**
   * @param name Const ref to algorithm's chain's first group
   * @param ID Unused
   */
  CounterSliceCPU::CounterSliceCPU(const TrigCostData* costData, const std::string& name, Int_t ID,
                                   UInt_t detailLevel, MonitorBase* parent) :
    CounterBase(costData, name, ID, detailLevel, parent) {
    m_dataStore.newVariable(kVarCalls).setSavePerCall().setSavePerEvent();
    m_dataStore.newVariable(kVarEventsActive).setSavePerCall().setSavePerEvent();
    m_dataStore.newVariable(kVarTime)
     .setSavePerCall("Slice Time Per Call;Slice Algorithm CPU Time [ms];Calls")
     .setSavePerEvent("Slice Time Per Event;Slice Algorithm CPU Time [ms];Events");
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterSliceCPU::~CounterSliceCPU() {
  }

  /**
   * Nothing
   */
  void CounterSliceCPU::startEvent() {
  }

  /**
   * Perform monitoring of a single algorithm call within a slice. Just CPU
   * @param e Sequence index in D3PD.
   * @param f Location of algorithm within parent sequence.
   * @param weight Event weight.
   */
  void CounterSliceCPU::processEventCounter(UInt_t e, UInt_t /*f*/, Float_t weight) {
    ++m_calls;

    m_dataStore.store(kVarCalls, 1., weight);
    m_dataStore.store(kVarTime, m_costData->getChainTimerFromSequences(e), weight);
  }

  /**
   * Perform end-of-event monitoring for this algorithm.
   */
  void CounterSliceCPU::endEvent(Float_t weight) {
    m_dataStore.store(kVarEventsActive, 1., /*m_eventWeight * */ weight);
    m_dataStore.endEvent();
  }

  Double_t CounterSliceCPU::getPrescaleFactor(UInt_t /*e*/) {
    return 0;
  }
} // namespace TrigCostRootAnalysis
