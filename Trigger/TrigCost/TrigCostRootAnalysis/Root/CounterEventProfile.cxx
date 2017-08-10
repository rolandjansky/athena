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
   * @param _name Const ref to name of the counter, by default this is of the form 'LumiBlock_xxx'.
   * @param _ID Lumi block this counter is monitoring
   */
  CounterEventProfile::CounterEventProfile(const TrigCostData* _costData, const std::string& _name, Int_t _ID,
                                           UInt_t _detailLevel, MonitorBase* _parent)
    : CounterBase(_costData, _name, _ID, _detailLevel, _parent) {
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
   * @param _e Unused
   * @param _f Unused
   * @param _weight Event weight.
   */
  void CounterEventProfile::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;
    _weight = 1.; // Don't use weighting

    if (Config::config().debug()) debug(_e);

    m_dataStore.store(kVarCalls, 1., _weight);
    m_dataStore.store(kVarAlgCaches, m_costData->getSeqAlgIsCached(_e, _f), _weight);
    m_dataStore.store(kVarROSCalls, m_costData->getSeqAlgROSCalls(_e, _f), _weight);
    m_dataStore.store(kVarROBReqSize, m_costData->getSeqAlgROBRequestSize(_e, _f), _weight);
    m_dataStore.store(kVarROBRetSize, m_costData->getSeqAlgROBRetrievalSize(_e, _f), _weight);
    Float_t _ROSTime = m_costData->getSeqAlgROSTime(_e, _f);
    m_dataStore.store(kVarROSTime, _ROSTime, _weight);
    m_dataStore.store(kVarCPUTime, m_costData->getSeqAlgTimer(_e, _f) - _ROSTime, _weight);

    Float_t _timeOffset = Config::config().getFloat(kEventTimeOffset);
    Float_t _time = m_costData->getSeqAlgTimeStartSec(_e, _f);
    _time += m_costData->getSeqAlgTimeStartMicroSec(_e, _f) / 1e6;
    _time -= _timeOffset;

    if (_time < 0.) {
      _time += 3600.; // Hour boundary crossed
      if (Config::config().getDisplayMsg(kMsgLargeSteerTime) == kTRUE) {
        Info("CounterEventProfile::processEventCounter", "Hour boundary crossed - corrected time is %f", _time);
      }
    }
    m_dataStore.store(kVarTimeElapsed, _time, _weight);
  }

  Double_t CounterEventProfile::getPrescaleFactor(UInt_t _e) {
    UNUSED(_e);
    return 1.;
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterEventProfile::endEvent(Float_t _weight) {
    UNUSED(_weight);
    m_dataStore.endEvent();
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterEventProfile::debug(UInt_t _e) {
    UNUSED(_e);
  }
} // namespace TrigCostRootAnalysis
