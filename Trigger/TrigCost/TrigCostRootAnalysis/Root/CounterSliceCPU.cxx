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
   * @param _name Const ref to algorithm's chain's first group
   * @param _ID Unused
   */
  CounterSliceCPU::CounterSliceCPU( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel, MonitorBase* _parent) :
    CounterBase(_costData, _name, _ID, _detailLevel, _parent) {

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
   * @param _e Sequence index in D3PD.
   * @param _f Location of algorithm within parent sequence.
   * @param _weight Event weight.
   */
  void CounterSliceCPU::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    UNUSED(_f);
    ++m_calls;

    m_dataStore.store(kVarCalls, 1., _weight);
    m_dataStore.store(kVarTime, m_costData->getChainTimerFromSequences(_e), _weight);
  }

  /**
   * Perform end-of-event monitoring for this algorithm.
   */
  void CounterSliceCPU::endEvent(Float_t _weight) {
    m_dataStore.store(kVarEventsActive, 1., /*m_eventWeight * */ _weight);
    m_dataStore.endEvent();
  }

  Double_t CounterSliceCPU::getPrescaleFactor(UInt_t _e) {
    UNUSED(_e);
    return 0;
  }


} // namespace TrigCostRootAnalysis
