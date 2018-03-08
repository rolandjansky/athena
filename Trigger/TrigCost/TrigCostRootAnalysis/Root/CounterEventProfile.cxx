// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <float.h> // FLT_MIN and FLT_MAX

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterEventProfile.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

namespace TrigCostRootAnalysis {
  /**
   * Evenr Profile counter - What is the typical event like? Can be broken down into differnt
   * @param name Const ref to name of the counter, by default this is of the form 'LumiBlock_xxx'.
   * @param ID Lumi block this counter is monitoring
   */
  CounterEventProfile::CounterEventProfile(const TrigCostData* costData, const std::string& name, Int_t ID,
                                           UInt_t detailLevel, MonitorBase* parent)
    : CounterBase(costData, name, ID, detailLevel, parent) {
    m_dataStore.newVariable(kVarCalls).setSavePerCall();
    m_dataStore.newVariable(kVarTimeElapsed).setSavePerCall();
    m_dataStore.newVariable(kVarCPUTime).setSavePerCall();
    m_dataStore.newVariable(kVarROSTime).setSavePerCall();
    m_dataStore.newVariable(kVarROSCalls).setSavePerCall();
    m_dataStore.newVariable(kVarAlgCaches).setSavePerCall();
    m_dataStore.newVariable(kVarROBReqSize).setSavePerCall();
    m_dataStore.newVariable(kVarROBRetSize).setSavePerCall();
  }

  /**
   * Counter destructor.
   */
  CounterEventProfile::~CounterEventProfile() {
  }

  /**
   * Reset per-event counter(s). Not used currently for EventProfile counter.
   */
  void CounterEventProfile::startEvent() {
  }

  /**
   * This alg was called at this counter's position in the execute sequence
   * @param e Unused
   * @param f Unused
   * @param weight Event weight.
   */
  void CounterEventProfile::processEventCounter(UInt_t e, UInt_t f, Float_t weight) {
    ++m_calls;
    weight = 1.; // Don't use weighting

    if (Config::config().debug()) debug(e);

    m_dataStore.store(kVarCalls, 1., weight);
    m_dataStore.store(kVarAlgCaches, m_costData->getSeqAlgIsCached(e, f), weight);
    m_dataStore.store(kVarROSCalls, m_costData->getSeqAlgROSCalls(e, f), weight);
    m_dataStore.store(kVarROBReqSize, m_costData->getSeqAlgROBRequestSize(e, f), weight);
    m_dataStore.store(kVarROBRetSize, m_costData->getSeqAlgROBRetrievalSize(e, f), weight);
    Float_t ROSTime = m_costData->getSeqAlgROSTime(e, f);
    m_dataStore.store(kVarROSTime, ROSTime, weight);
    m_dataStore.store(kVarCPUTime, m_costData->getSeqAlgTimer(e, f) - ROSTime, weight);

    Float_t timeOffset = Config::config().getFloat(kEventTimeOffset);
    Float_t time = m_costData->getSeqAlgTimeStartSec(e, f);
    time += m_costData->getSeqAlgTimeStartMicroSec(e, f) / 1e6;
    time -= timeOffset;

    if (time < 0.) {
      time += 3600.; // Hour boundary crossed
      if (Config::config().getDisplayMsg(kMsgLargeSteerTime) == kTRUE) {
        Info("CounterEventProfile::processEventCounter", "Hour boundary crossed - corrected time is %f", time);
      }
    }
    m_dataStore.store(kVarTimeElapsed, time, weight);
  }

  Double_t CounterEventProfile::getPrescaleFactor(UInt_t /*e*/) {
    return 1.;
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterEventProfile::endEvent(Float_t /*weight*/) {
    m_dataStore.endEvent();
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterEventProfile::debug(UInt_t /*e*/) {
  }
} // namespace TrigCostRootAnalysis
