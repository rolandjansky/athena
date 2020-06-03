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
   * @param name Const ref to name of the counter, by default this is of the form 'LumiBlock_xxx'.
   * @param ID Lumi block this counter is monitoring
   */
  CounterGlobals::CounterGlobals(const TrigCostData* costData, const std::string& name, Int_t ID,
                                 UInt_t detailLevel, MonitorBase* parent)
    : CounterBase(costData, name, ID, detailLevel, parent),
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

    decorate(kDecLBMuValue, TrigXMLService::trigXMLService().getLBMuValue(ID));

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
   * @param e Unused
   * @param f Unused
   * @param weight Event weight.
   */
  void CounterGlobals::processEventCounter(UInt_t e, UInt_t /*f*/, Float_t weight) {
    ++m_calls;
    static Bool_t invertFilter = (Bool_t) Config::config().getInt(kPatternsInvert);

    m_earliestTimestamp = FLT_MAX;
    m_latestTimestamp = FLT_MIN;
    m_steeringTime = 0.;

    m_dataStore.store(kVarEventsActive, 1., weight);

    //Did L1 pass?
    for (UInt_t i = 0; i < m_costData->getNL1(); ++i) {
      if (m_costData->getIsL1PassedAfterVeto(i) == kFALSE) continue;
      m_dataStore.store(kVarL1PassEvents, 1., weight);
      break;
    }

    //Did HLT run? (almost certainly... but check)
    if (m_costData->getNChains()) m_dataStore.store(kVarHLTEvents, 1., weight);

    //Did HLT pass?
    const Bool_t hltPass = (Bool_t) Config::config().getInt(kHLTPass);
    if (hltPass) m_dataStore.store(kVarHLTPassEvents, 1., weight);

    // Look at all algs in this event
    Int_t havePatterns = Config::config().getVecSize(kPatternsMonitor);
    for (UInt_t s = 0; s < m_costData->getNSequences(); ++s) {
      // Loop over all algorithms in sequence
      Bool_t isRerun = m_costData->getSeqIsRerun(s);
      for (UInt_t a = 0; a < m_costData->getNSeqAlgs(s); ++a) {
        Float_t algWeight = weight * getPrescaleFactor(e);
        if (isZero(algWeight) == kTRUE) continue;

        if (havePatterns > 0) {
          Int_t chainID = m_costData->getSequenceChannelCounter(s);
          const std::string chainName = TrigConfInterface::getHLTNameFromChainID(chainID);
          if (checkPatternNameMonitor(chainName, invertFilter, m_costData->getSeqIsRerun(s)) == kFALSE) continue;
        }

        m_dataStore.store(kVarAlgCalls, 1., algWeight);
        m_dataStore.store(kVarROSCalls, m_costData->getSeqAlgROSCalls(s, a), algWeight);
        m_dataStore.store(kVarAlgTime, m_costData->getSeqAlgTimer(s, a), algWeight);
        m_dataStore.store(kVarROSTime, m_costData->getSeqAlgROSTime(s, a), algWeight);
        m_dataStore.store(kVarCPUTime, m_costData->getSeqAlgTimer(s, a) - m_costData->getSeqAlgROSTime(s, a), algWeight);

        if (isRerun) m_dataStore.store(kVarRerunTime, m_costData->getSeqAlgTimer(s, a), algWeight);
        if (hltPass) m_dataStore.store(kVarPassTime, m_costData->getSeqAlgTimer(s, a), algWeight);

        // Calculate the start and stop from the steering info
        if (!isZero(m_costData->getSeqAlgTimeStart(s, a)) &&
            m_costData->getSeqAlgTimeStart(s, a) < m_earliestTimestamp) {
          m_earliestTimestamp = m_costData->getSeqAlgTimeStart(s, a);
        }
        if (m_costData->getSeqAlgTimeStop(s, a) > m_latestTimestamp) {
          m_latestTimestamp = m_costData->getSeqAlgTimeStop(s, a);
        }

        if (Config::config().debug()) {
          m_algTime += m_costData->getSeqAlgTimer(s, a);
          m_rosTime += m_costData->getSeqAlgROSTime(s, a);
          m_cpuTime += m_costData->getSeqAlgTimer(s, a) - m_costData->getSeqAlgROSTime(s, a);
          m_rosCalls += m_costData->getSeqAlgROSCalls(s, a);
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
    m_dataStore.store(kVarSteeringTime, m_steeringTime, weight);

    m_dataStore.store(kVarROI, m_costData->getNRoIs(), weight);

    // Did we encounter a new processing unit? Count unique PUs
    if (m_processingUnits.count(m_costData->getAppId()) == 0) m_dataStore.store(kVarHLTPUs, 1.);
    m_processingUnits[ m_costData->getAppId() ] += 1;

    if (m_CPUBreakDown == kTRUE) {
      Int_t computerRack = TrigXMLService::trigXMLService().getComputerType(((UInt_t) m_costData->getAppId()));
      if (computerRack >= 0 && computerRack <= 90) {
        m_dataStore.store(ConfKey_t(kVarSteeringTimeCPUType + 8192 + computerRack), m_steeringTime, weight);
        m_dataStore.store(ConfKey_t(kVarEventsCPUType + 16384 + computerRack), 1., weight);
      }
    }

    // Misc event timers
    m_dataStore.store(kVarTrigCostTime, m_costData->getTimerTrigCost(), 1.); // Note unweighted as this correlates 100%
                                                                             // with selected events to monitor
    m_dataStore.store(kVarTexecTime, m_costData->getTimerEndSteer(), weight);
    m_dataStore.store(kVarChainExecTime, m_costData->getTimerChainProcessed(), weight);
    m_dataStore.store(kVarResultBuildingTime, m_costData->getTimerResultBuilder(), weight);
    m_dataStore.store(kVarMonitoringTime, m_costData->getTimerMonitoring(), weight);

    if (Config::config().debug()) debug(0);
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterGlobals::endEvent(Float_t /*weight*/) {
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
  Double_t CounterGlobals::getPrescaleFactor(UInt_t e) {
    return TrigXMLService::trigXMLService().getHLTCostWeightingFactor(
      TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(e),
                                               m_costData->getSequenceLevel(e)));
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterGlobals::debug(UInt_t /*e*/) {

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
