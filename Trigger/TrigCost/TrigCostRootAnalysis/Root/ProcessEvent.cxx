// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <map>
#include <utility> //std::pair
#include <assert.h>
#include <time.h>
#include <thread> //c++11

//Local include(s):
#include "../TrigCostRootAnalysis/ProcessEvent.h"
#include "../TrigCostRootAnalysis/MonitorBase.h"
#include "../TrigCostRootAnalysis/MonitorChain.h"
#include "../TrigCostRootAnalysis/MonitorSequence.h"
#include "../TrigCostRootAnalysis/MonitorAlgorithm.h"
#include "../TrigCostRootAnalysis/MonitorAlgorithmSequence.h"
#include "../TrigCostRootAnalysis/MonitorAlgorithmChain.h"
#include "../TrigCostRootAnalysis/MonitorAlgorithmClass.h"
#include "../TrigCostRootAnalysis/MonitorROS.h"
#include "../TrigCostRootAnalysis/MonitorROSAlgorithm.h"
#include "../TrigCostRootAnalysis/MonitorROSChain.h"
#include "../TrigCostRootAnalysis/MonitorROBIN.h"
#include "../TrigCostRootAnalysis/MonitorFullEvent.h"
#include "../TrigCostRootAnalysis/MonitorROI.h"
#include "../TrigCostRootAnalysis/MonitorGlobals.h"
#include "../TrigCostRootAnalysis/MonitorEventProfile.h"
#include "../TrigCostRootAnalysis/MonitorRates.h"
#include "../TrigCostRootAnalysis/MonitorRatesUpgrade.h"
#include "../TrigCostRootAnalysis/MonitorSliceCPU.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/EnergyExtrapolation.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

namespace TrigCostRootAnalysis {
  /**
   * ProcessEvent constructor. Not much here.
   */
  ProcessEvent::ProcessEvent(const TrigCostData* costData, UInt_t level, const std::string& name)
    : m_costData(costData), m_level(level), m_name(name),
    m_runNumber(0),
    m_invertHighMuRunVeto(kFALSE),
    m_threadTimer("Event", "Thread-Spawning"),
    m_takeEventTimer("Event", "Classifying Event"),
    m_cacheAlgTimer("Event", "Alg Monitor Caching"),
    m_cacheROSTimer("Event", "ROS Monitor Caching"),
    m_dataPrepTimer("Event", "Data Preperation"),
    m_processTime("Event", "Executing Monitors"),
    m_needsHLTPassInformation(kFALSE) {
    m_costData->setParent(this);
    m_nThread = Config::config().getInt(kNThread);
    m_threadFnPtr = &newEventThreaded;
    m_ratesOnly = Config::config().getInt(kRatesOnly);
    m_isCPUPrediction = (Bool_t) Config::config().getInt(kIsCPUPrediction);
    m_useOnlyTheseBCIDs = &(Config::config().getIntVec(kUseOnlyTheseBCIDs));
    m_invertFilter = Config::config().getInt(kPatternsInvert);
    m_pass = 0;
  }

  /**
   * ProcessEvent destructor. Clear memory.
   */
  ProcessEvent::~ProcessEvent() {
    // Good housekeeping
    monitorIt_t it = m_monitorCollections.begin();

    for (; it != m_monitorCollections.end(); ++it) {
      delete it->second;
    }
    m_monitorCollections.clear();
  }

  /**
   * Used to enable or disable individual monitoring modes. All start out disabled.
   * Modes which were enabled but are subsequently disabled are deleted and their memory freed.
   * @see MonitorType_t
   * @param type Enum of which monitor type to act on. Can be the wild-card kAllMonitors
   * @param isActive Boolean which specifies if monitor is to be enabled or disabled.
   */
  void ProcessEvent::setMonitoringMode(ConfKey_t type, Bool_t isActive) {
    assert(type > kMonitorBegin && type <= kDoAllMonitor);

    // If setting all, then recurs this function
    if (type == kDoAllMonitor) {
      for (UInt_t t = kMonitorBegin + 1; t < kDoAllMonitor; ++t) { // Avoid hitting kAllMonitors again
        setMonitoringMode((ConfKey_t) t, isActive);
      }
      return;
    }

    // Find this entry in the map (may not be present)
    // This must be done by hand as object need to be of correct derived type
    monitorNonConstIt_t it = m_monitorCollections.find(type);
    if (isActive == false) {
      // If disabling, check if present and delete if found
      if (it != m_monitorCollections.end()) {
        delete it->second;
        it->second = 0;
        m_monitorCollections.erase(it);
        Info("ProcessEvent::setMonitoringMode", "Disabling monitoring Level:%s Mode:%s",
             getLevelStr().c_str(), Config::config().getName(type).c_str());
      }
    } else {
      // If enabling, check if present and add if not
      if (type == kDoRatesMonitor
          && (getLevelStr() == Config::config().getStr(kL2String)
              || getLevelStr() == Config::config().getStr(kEFString))) {
        Warning("ProcessEvent::setMonitoringMode", "Rates monitoring not yet implimented for \"old style\" L2 or EF");
        return;
      }
      if (it == m_monitorCollections.end()) {
        MonitorBase* costMonitor;
        // Create object of appropriate derived type
        switch (type) {
        case kDoChainMonitor:
          costMonitor = new MonitorChain(m_costData);
          m_needsHLTPassInformation = kTRUE;
          break;

        case kDoChainAlgorithmMonitor:
          costMonitor = new MonitorAlgorithmChain(m_costData);
          break;

        case kDoSequenceMonitor:
          costMonitor = new MonitorSequence(m_costData);
          break;

        case kDoSequenceAlgorithmMonitor:
          costMonitor = new MonitorAlgorithmSequence(m_costData);
          break;

        case kDoAlgorithmMonitor:
          costMonitor = new MonitorAlgorithm(m_costData);
          break;

        case kDoAlgorithmClassMonitor:
          costMonitor = new MonitorAlgorithmClass(m_costData);
          break;

        case kDoROSMonitor:
          costMonitor = new MonitorROS(m_costData);
          break;

        case kDoROBINMonitor:
          costMonitor = new MonitorROBIN(m_costData);
          break;

        case kDoROSAlgorithmMonitor:
          costMonitor = new MonitorROSAlgorithm(m_costData);
          break;

        case kDoROSChainMonitor:
          costMonitor = new MonitorROSChain(m_costData);
          break;

        case kDoFullEventMonitor:
          costMonitor = new MonitorFullEvent(m_costData);
          break;

        case kDoROIMonitor:
          costMonitor = new MonitorROI(m_costData);
          break;

        case kDoGlobalsMonitor:
          costMonitor = new MonitorGlobals(m_costData);
          m_needsHLTPassInformation = kTRUE;
          break;

        case kDoEventProfileMonitor:
          costMonitor = new MonitorEventProfile(m_costData);
          break;

        case kDoRatesMonitor:
          costMonitor = new MonitorRates(m_costData);
          break;

        case kDoRatesUpgradeMonitor:
          costMonitor = new MonitorRatesUpgrade(m_costData);
          break;

        case kDoSliceCPUMonitor:
          costMonitor = new MonitorSliceCPU(m_costData);
          break;

        default:
          Error("ProcessEvent::setMonitoringMode", "Unknown or unimplemented Monitor Type with enum:%i", type);
          return;
        }
        Info("ProcessEvent::setMonitoringMode", "Enabling monitoring Level:%s Mode:%s",
             getLevelStr().c_str(), Config::config().getName(type).c_str());
        costMonitor->setLevel(getLevel());
        m_monitorCollections.insert(monitorPair_t(type, costMonitor));
      }
    }
  }

  /**
   * Static function to spawn threaded monitor execution
   * @param The monitor to execute in this thread
   * @param The weight to pass it
   */
  void ProcessEvent::newEventThreaded(MonitorBase* monitor, Float_t weight) {
    monitor->newEvent(weight);
  }

  /**
   * Call once per event, passing the cost data and the event weight. This information is fed down in turn to all
   * registered and active monitors such that they may tabulate the event details.
   * We also apply here and enhanced bias or energy extrapolation weights which affect the whole event
   * @param costData Const reference to cost data object.
   * @param weight Event weight.
   * @return kTRUE if at least one monitor reported as having executed on the event.
   */
  Bool_t ProcessEvent::newEvent(Float_t weight) {
    //Check for weights from energy extrapolation
    weight *= EnergyExtrapolation::energyExtrapolation().getEventWeight(m_costData);

    if (m_runNumber == 0) {
      m_runNumber = Config::config().getInt(kRunNumber);
      m_invertHighMuRunVeto = Config::config().getInt(kInvertHighMuRunVeto);
    }
    // Special behaviour for the 2016 high mu run
    if (m_runNumber == 310574) {
      const UInt_t bcid = m_costData->getBunchCrossingId();
      const Bool_t isolatedBunch = (bcid == 11 || bcid == 1247 || bcid == 2430);
      if (m_invertHighMuRunVeto == kFALSE && isolatedBunch == kTRUE) return false;
      // We normally veto on these isolated bunches
      if (m_invertHighMuRunVeto == kTRUE && isolatedBunch == kFALSE) return false;
      // Or if inverted, we only keep the isolated bunches
    }
    // Special BCID filter
    if (m_useOnlyTheseBCIDs->size() > 0) {
      if (std::find(m_useOnlyTheseBCIDs->begin(), m_useOnlyTheseBCIDs->end(),
                    (Int_t) m_costData->getBunchCrossingId()) == m_useOnlyTheseBCIDs->end()) {
        return false;
      }
    }

    //Check for enhanced bias weights.
    if (Config::config().getInt(kDoEBWeighting) == kTRUE) {
      weight *= TrigXMLService::trigXMLService().getEventWeight(m_costData->getEventNumber(),
                                                                 m_costData->getLumi(), getPass());
    }

    // For each active monitoring type, process event
    if (isZero(weight) == kTRUE) return false;


    // Do any monitors want to take this event?
    m_takeEventTimer.start();
    Int_t takeEvent = kFALSE;
    for (monitorIt_t it = m_monitorCollections.begin(); it != m_monitorCollections.end(); ++it) {
      takeEvent += it->second->getNCollectionsToProcess();
      // Note we cannot break this loop early, all monitors need to get this call to prepare their list of collections
      // to process
      // and perform bookkeeping
    }
    m_takeEventTimer.stop();
    if (takeEvent == 0) return kFALSE;

    // Do buffering
    if (m_ratesOnly == kFALSE) {
      m_dataPrepTimer.start();
      m_costData->bufferEvent();
      m_dataPrepTimer.stop();

      m_cacheAlgTimer.start();
      MonitorAlgorithmCommon::getAllAlgsInEvent(getLevel(), m_costData);
      m_cacheAlgTimer.stop();
      m_cacheROSTimer.start();
      MonitorROSCommon::collateROSRequests(getLevel(), m_costData);
      //MonitorROBIN::collateROBINRequests(getLevel(), m_costData);
      m_cacheROSTimer.stop();
    }


    // Special - CPU prediction mode
    // We demand a chain which was not the costmonitor chain (or any other "misbehaving" streamers etc.) to pass physics
    // physics chain (NOT rerun).
    // Otherwise rerun on the costmon chain will mess up the prediction
    if (m_isCPUPrediction) {
      UInt_t chainPasses = 0;
      for (UInt_t c = 0; c < m_costData->getNChains(); ++c) {
        const std::string chainName = TrigConfInterface::getHLTNameFromChainID(m_costData->getChainID(
                                                                                  c), m_costData->getChainLevel(c));
        if (Config::config().getVecMatches(kPatternsMonitor, chainName) == kTRUE) continue;
        chainPasses += (Int_t) m_costData->getIsChainPassed(c);
        if (chainPasses > 0) break;
      }
      static const std::string ign = "IgnoreRerun";
      if (chainPasses == 0) Config::config().set(kIgnoreRerun, 1, ign, kUnlocked);
      // Then ignore RERUN chains in this event.
      else Config::config().set(kIgnoreRerun, 0, ign, kUnlocked);
    }

    // Figure out if the event was accepted for physics
    if (m_needsHLTPassInformation) {
      //Did HLT pass?
      Int_t hltPass = 0;
      for (UInt_t i = 0; i < m_costData->getNChains(); ++i) {
        if (m_costData->getIsChainPassed(i) == kFALSE) {
          continue; // I didn't passed
        }
        if (m_costData->getIsChainResurrected(i) == kTRUE) {
          continue; // I was rerun (cannot cause event to be accepted)
        }
        const std::string chainName = TrigConfInterface::getHLTNameFromChainID(m_costData->getChainID(i));
        if (TrigConfInterface::getChainIsMainStream(chainName) == kFALSE) {
          continue; // Only look at physics accepts
        }
        if (checkPatternNameMonitor(chainName, m_invertFilter, m_costData->getIsChainResurrected(i)) == kFALSE) continue;
        hltPass = 1;
        break;
      }
      static const std::string passStr = "HLTPass";
      Config::config().set(kHLTPass, hltPass, passStr, kUnlocked);
    }

    if (m_nThread == 1 || m_ratesOnly == kTRUE) {
      // Non threaded
      m_processTime.start();
      for (monitorIt_t it = m_monitorCollections.begin(); it != m_monitorCollections.end(); ++it) {
        it->second->newEvent(weight);
      }
      m_processTime.stop();
    } else {
      // Threaded
      std::vector<std::thread> executing;
      std::vector<MonitorBase*> notThreadable; // Vector of function pointers

      //m_threadTimer.start();
      //m_threadTimer.stop();

      m_processTime.start();
      monitorIt_t it = m_monitorCollections.begin();
      Bool_t doneAll = kFALSE;

      do {
        while (executing.size() < m_nThread && doneAll == kFALSE) { // Loop until full of threads or no more to add
          MonitorBase* monitorInstance = it->second;
          if (it->second->isThreadable() == kTRUE) {
            // Spawn a new thread to call m_threadFnPtr to run monitorInstance with argument weight
            executing.push_back(std::thread(m_threadFnPtr, monitorInstance, weight));
          } else { // Otherwise we execute it later
            notThreadable.push_back(monitorInstance);
          }
          if (++it == m_monitorCollections.end()) {
            doneAll = kTRUE; // Or break if the last monitor
            break;
          }
        }

        // Wait for last thread to halt
        if (executing.size() > 0) {
          executing.back().join();
          executing.pop_back();
        }
      } while (doneAll == kFALSE || executing.size() > 0);

      // Do un-threadable after. Call the functions
      for (UInt_t m = 0; m < notThreadable.size(); ++m) {
        notThreadable.at(m)->newEvent(weight);
      }
      m_processTime.stop();
    }
    return kTRUE;
  }

  /**
   * Sets which pass through the input file(s) we're on down to all the monitors
   * @param pass Number of this pass
   **/
  void ProcessEvent::setPass(UInt_t pass) {
    for (const auto& monitor : m_monitorCollections) monitor.second->setPass(pass);
    m_pass = pass;
  }

  /**
   *  @return Which number pass through the input file(s)
   **/
  UInt_t ProcessEvent::getPass() {
    return m_pass;
  }

  /**
   * Gets a map of the internal monitor objects. Note that while these are stored as base pointers, they are all
   * in fact polymorphic treatments of the derived classes.
   * @see setMonitoringMode(MonitorType_t type, Bool_t isActive)
   * @returns Map of MonitorType_t enums to TrigMonitorBase pointers.
   */
  const monitorMap_t& ProcessEvent::getMonitors() {
    return m_monitorCollections;
  }

  /**
   * Get base class pointer to a requested monitor. This can then be cast to the appropriate type if specific
   * base class dependent functionality is required.
   * @param type The type of monitor to retrieve.
   * @result Base-class pointer to object. Returns 0 if monitor is not present.
   */
  MonitorBase* ProcessEvent::getMonitor(ConfKey_t type) {
    assert(type >= kDoChainMonitor && type < kDoAllMonitor);
    monitorIt_t it = m_monitorCollections.find(type);
    if (it != m_monitorCollections.end()) {
      return it->second;
    } else {
      Error("ProcessEvent::getMonitor", "Collection %s is not registered.", Config::config().getName(type).c_str());
      return 0;
    }
  }

  /**
   * Easy way of instigating the end-of-execution output sequence. The saveOutput method is called on all
   * currently active monitors.
   */
  void ProcessEvent::saveOutput() {
    // Execute end-of-run code
    for (monitorIt_t it = m_monitorCollections.begin(); it != m_monitorCollections.end(); ++it) {
      UInt_t nCounters = it->second->getNCounters();
      if (nCounters == 0) {
        Info("ProcessEvent::saveOutput",
             "Skipping end-of-run output for %s %s monitor. Monitor did not find any data to run over.",
             getLevelStr().c_str(),
             it->second->getName().c_str());
        continue;
      }
      Info("ProcessEvent::saveOutput", "%s %s monitor processed %i counters. Writing out counters...",
           getLevelStr().c_str(),
           it->second->getName().c_str(),
           nCounters);
      it->second->saveOutput();
    }
  }

  /**
   * @param level The HLT level of this processor to set.
   */
  void ProcessEvent::setLevel(UInt_t level) {
    m_level = level;
  }

  /**
   * @returns The HLT level of this processor.
   */
  UInt_t ProcessEvent::getLevel() {
    return m_level;
  }

  /**
   * Get a string version of the level this event processor is operating at.
   * @returns Process level as string, "L2, "EF" or "HLT".
   */
  const std::string& ProcessEvent::getLevelStr() {
    return getLevelString(getLevel());
  }
} // namespace TrigCostRootAnalysis
