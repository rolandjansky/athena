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
#include "../TrigCostRootAnalysis/CounterGlobals.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

namespace TrigCostRootAnalysis {
  /**
   * Global quantities counter - monitor overall HLT processing for a single LB
   * @param _name Const ref to name of the counter, by default this is of the form 'LumiBlock_xxx'.
   * @param _ID Lumi block this counter is monitoring
   */
  CounterGlobals::CounterGlobals(const TrigCostData* _costData, const std::string& _name, Int_t _ID,
                                 UInt_t _detailLevel, MonitorBase* _parent)
    : CounterBase(_costData, _name, _ID, _detailLevel, _parent),
    m_earliestTimestamp(0.),
    m_latestTimestamp(0.),
    m_steeringTime(0.),
    m_algTime(0.),
    m_rosTime(0.),
    m_cpuTime(0.),
    m_rosCalls(0),
    m_algCalls(0),
    m_CPUBreakDown(0) {
    m_dataStore.newVariable(kVarEventsActive).setSavePerCall();

    m_dataStore.newVariable(kVarL1PassEvents).setSavePerCall();

    m_dataStore.newVariable(kVarHLTEvents).setSavePerCall();

    m_dataStore.newVariable(kVarHLTPassEvents).setSavePerCall();

    m_dataStore.newVariable(kVarHLTPUs).setSavePerCall();

    m_dataStore.newVariable(kVarSteeringTime).setSavePerEvent("Steering Time Per Event;Steering Time [ms];Events");

    m_dataStore.newVariable(kVarAlgTime)
     .setSavePerCall("Algorithm WallTime Per Algorithm Call;Algorithm Time [ms];Calls")
     .setSavePerEvent("Algorithm WallTime Per Event;Algorithm Time [ms];Events");

    m_dataStore.newVariable(kVarRerunTime).setSavePerEvent();

    m_dataStore.newVariable(kVarPassTime).setSavePerEvent();

    m_dataStore.newVariable(kVarAlgCalls).setSavePerEvent().setSavePerCall(); //TODO remove this latter one

    m_dataStore.newVariable(kVarROSTime).setSavePerEvent("Readout System Time Per Event;ROS Time [ms];Events");

    m_dataStore.newVariable(kVarCPUTime).setSavePerEvent("Algorithm CPU Time Per Event;CPU Time [ms];Events");

    m_dataStore.newVariable(kVarROSCalls).setSavePerEvent("Readout System Calls Per Event;ROS Calls;Events");

    m_dataStore.newVariable(kVarROI).setSavePerEvent("Number of Regions of Interest Per Event;RoIs;Events");

    m_dataStore.newVariable(kVarTrigCostTime).setSavePerEvent(
      "Time Taken by CostMonitoring Tool Itself Per Event;Time [ms];Events");

    m_dataStore.newVariable(kVarTexecTime).setSavePerEvent("Time Taken by TExec Timer Per Event;Time [ms];Events");

    m_dataStore.newVariable(kVarChainExecTime).setSavePerEvent(
      "Time Taken by Chain Execution Per Event;Time [ms];Events");

    m_dataStore.newVariable(kVarResultBuildingTime).setSavePerEvent(
      "Time Taken by Result Builder Per Event;Time [ms];Events");

    m_dataStore.newVariable(kVarMonitoringTime).setSavePerEvent(
      "Time Taken by Monitoring Tools Per Event;Time [ms];Events");

    TrigXMLService::trigXMLService().parseHLTFarmXML();
    for (int i = 0; i <= 90; ++i) {
      m_dataStore.newVariable(ConfKey_t(kVarSteeringTimeCPUType + 8192 + i)).setSavePerEvent(std::string("Steering Time Per Event by Rack " +
                                                                                    (i == 0 ? "UNKNOWN" : intToString(i)) +
                                                                                    ";Steering Time [ms];Events"));
      m_dataStore.newVariable(ConfKey_t(kVarEventsCPUType + 16384 + i)).setSavePerCall();
      m_CPUBreakDown = kTRUE;
    }
  }

  /**
   * Counter destructor.
   */
  CounterGlobals::~CounterGlobals() {
  }

  /**
   * Reset per-event counter(s). Not used currently for globals counter.
   */
  void CounterGlobals::startEvent() {
  }

  /**
   * Add this event to the global monitor.
   * I am run once for every event in my assigned lumi block
   * @param _e Unused
   * @param _f Unused
   * @param _weight Event weight.
   */
  void CounterGlobals::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;
    UNUSED(_e);
    UNUSED(_f);
    static Bool_t _invertFilter = (Bool_t) Config::config().getInt(kPatternsInvert);

    m_earliestTimestamp = FLT_MAX;
    m_latestTimestamp = FLT_MIN;
    m_steeringTime = 0.;

    m_dataStore.store(kVarEventsActive, 1., _weight);

    //Did L1 pass?
    for (UInt_t _i = 0; _i < m_costData->getNL1(); ++_i) {
      if (m_costData->getIsL1PassedAfterVeto(_i) == kFALSE) continue;
      m_dataStore.store(kVarL1PassEvents, 1., _weight);
      break;
    }

    //Did HLT run? (almost certainly... but check)
    if (m_costData->getNChains()) m_dataStore.store(kVarHLTEvents, 1., _weight);

    //Did HLT pass?
    Bool_t _hltPass = kFALSE;
    for (UInt_t _i = 0; _i < m_costData->getNChains(); ++_i) {
      if (m_costData->getIsChainPassed(_i) == kFALSE) continue;
      const std::string _chainName = TrigConfInterface::getHLTNameFromChainID(m_costData->getChainID(_i));
      if (_chainName.find("costmonitor") != std::string::npos) continue;                                                                              
      // This always passes!
      if (checkPatternNameMonitor(_chainName, _invertFilter, m_costData->getIsChainResurrected(_i)) == kFALSE) continue;

      m_dataStore.store(kVarHLTPassEvents, 1., _weight);
      _hltPass = kTRUE;
      break;
    }

    // Look at all algs in this event
    Int_t _havePatterns = Config::config().getVecSize(kPatternsMonitor);
    for (UInt_t _s = 0; _s < m_costData->getNSequences(); ++_s) {
      // Loop over all algorithms in sequence
      Bool_t _isRerun = m_costData->getSeqIsRerun(_s);
      for (UInt_t _a = 0; _a < m_costData->getNSeqAlgs(_s); ++_a) {
        Float_t _algWeight = _weight * getPrescaleFactor(_e);
        if (isZero(_algWeight) == kTRUE) continue;

        if (_havePatterns > 0) {
          Int_t _chainID = m_costData->getSequenceChannelCounter(_s);
          const std::string _chainName = TrigConfInterface::getHLTNameFromChainID(_chainID);
          if (checkPatternNameMonitor(_chainName, _invertFilter, m_costData->getSeqIsRerun(_s)) == kFALSE) continue;
        }

        m_dataStore.store(kVarAlgCalls, 1., _algWeight);
        m_dataStore.store(kVarROSCalls, m_costData->getSeqAlgROSCalls(_s, _a), _algWeight);
        m_dataStore.store(kVarAlgTime, m_costData->getSeqAlgTimer(_s, _a), _algWeight);
        m_dataStore.store(kVarROSTime, m_costData->getSeqAlgROSTime(_s, _a), _algWeight);
        m_dataStore.store(kVarCPUTime, m_costData->getSeqAlgTimer(_s, _a) - m_costData->getSeqAlgROSTime(_s, _a), _algWeight);

        if (_isRerun) m_dataStore.store(kVarRerunTime, m_costData->getSeqAlgTimer(_s, _a), _algWeight);
        if (_hltPass) m_dataStore.store(kVarPassTime, m_costData->getSeqAlgTimer(_s, _a), _algWeight);

        // Calculate the start and stop from the steering info
        if (!isZero(m_costData->getSeqAlgTimeStart(_s, _a)) &&
            m_costData->getSeqAlgTimeStart(_s, _a) < m_earliestTimestamp) {
          m_earliestTimestamp = m_costData->getSeqAlgTimeStart(_s, _a);
        }
        if (m_costData->getSeqAlgTimeStop(_s, _a) > m_latestTimestamp) {
          m_latestTimestamp = m_costData->getSeqAlgTimeStop(_s, _a);
        }

        if (Config::config().debug()) {
          m_algTime += m_costData->getSeqAlgTimer(_s, _a);
          m_rosTime += m_costData->getSeqAlgROSTime(_s, _a);
          m_cpuTime += m_costData->getSeqAlgTimer(_s, _a) - m_costData->getSeqAlgROSTime(_s, _a);
          m_rosCalls += m_costData->getSeqAlgROSCalls(_s, _a);
          ++m_algCalls;
        }
      }
    }

    m_steeringTime = (m_latestTimestamp - m_earliestTimestamp);
    if (m_earliestTimestamp == FLT_MAX || m_latestTimestamp == FLT_MIN) m_steeringTime = 0.; //Check we got both a start
                                                                                             // and an end
    // Time variable goes up to 1 hour (3600s). We need to check for the case where event execution happens over the 1h
    // mark.
    if (m_steeringTime > 1800.) { // If steering time apparently over 30m
      m_steeringTime = 3600 - m_steeringTime;
      if (Config::config().getDisplayMsg(kMsgLargeSteerTime) == kTRUE) {
        Warning("CounterGlobals::processEventCounter",
                "Excessivly large Steering Time Stamp - subtracting one hour: lowStamp%f, highStamp:%f, steeringTime:%f",
                m_earliestTimestamp,
                m_latestTimestamp,
                m_steeringTime);
      }
    }

    // Convert to milisec
    m_steeringTime *= 1000.;
    // Slow event? This sets a flag which other counters will use
    if (m_steeringTime > Config::config().getInt(kSlowEvThreshold)) Config::config().set(kCurrentEventIsSlow, 1, kUnlocked);
    else Config::config().set(kCurrentEventIsSlow, 0, kUnlocked);
    m_dataStore.store(kVarSteeringTime, m_steeringTime, _weight);

    m_dataStore.store(kVarROI, m_costData->getNRoIs(), _weight);

    // Did we encounter a new processing unit? Count unique PUs
    if (m_processingUnits.count(m_costData->getAppId()) == 0) m_dataStore.store(kVarHLTPUs, 1.);
    m_processingUnits[ m_costData->getAppId() ] += 1;

    if (m_CPUBreakDown == kTRUE) {
      Int_t _computerRack = TrigXMLService::trigXMLService().getComputerType(((UInt_t) m_costData->getAppId()));
      if (_computerRack >= 0 && _computerRack <= 90) {
        m_dataStore.store(ConfKey_t(kVarSteeringTimeCPUType + 8192 + _computerRack), m_steeringTime, _weight);
        m_dataStore.store(ConfKey_t(kVarEventsCPUType + 16384 + _computerRack), 1., _weight);
      }
    }

    // Misc event timers
    m_dataStore.store(kVarTrigCostTime, m_costData->getTimerTrigCost(), 1.); // Note unweighted as this correlates 100%
                                                                             // with selected events to monitor
    m_dataStore.store(kVarTexecTime, m_costData->getTimerEndSteer(), _weight);
    m_dataStore.store(kVarChainExecTime, m_costData->getTimerChainProcessed(), _weight);
    m_dataStore.store(kVarResultBuildingTime, m_costData->getTimerResultBuilder(), _weight);
    m_dataStore.store(kVarMonitoringTime, m_costData->getTimerMonitoring(), _weight);

    if (Config::config().debug()) debug(0);
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterGlobals::endEvent(Float_t _weight) {
    UNUSED(_weight);
    m_dataStore.endEvent();
    if (Config::config().debug()) {
      m_algTime = 0;
      m_rosTime = 0;
      m_cpuTime = 0;
      m_rosCalls = 0;
      m_algCalls = 0;
    }
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * Nees to be same as for alg
   * @return Multiplicitive weighting factor
   */
  Double_t CounterGlobals::getPrescaleFactor(UInt_t _e) {
    return TrigXMLService::trigXMLService().getHLTCostWeightingFactor(
      TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(_e),
                                               m_costData->getSequenceLevel(_e)));
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterGlobals::debug(UInt_t _e) {
    UNUSED(_e);

    Info("CounterGlobals::debug", "Calls:%i, lowStamp%f, highStamp:%f, steeringTime:%f, algTime:%f,"
                                  "cpuTime:%f, rosTime:%f, rosCalls:%i, rosInFile:%i, rois:%i, algs:%i",
         m_calls,
         m_earliestTimestamp,
         m_latestTimestamp,
         m_steeringTime,
         m_algTime,
         m_cpuTime,
         m_rosTime,
         m_rosCalls,
         m_costData->getNROBs(),
         m_costData->getNRoIs(),
         m_algCalls);
  }
} // namespace TrigCostRootAnalysis
