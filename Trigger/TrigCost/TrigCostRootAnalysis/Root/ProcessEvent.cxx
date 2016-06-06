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
  ProcessEvent::ProcessEvent(const TrigCostData* _costData, UInt_t _level, const std::string& _name) 
    : m_costData(_costData), m_level(_level), m_name(_name), 
      m_threadTimer("Event", "Thread-Spawning"), 
      m_takeEventTimer("Event", "Classifying Event"),
      m_cacheAlgTimer("Event", "Alg Monitor Caching"),
      m_cacheROSTimer("Event", "ROS Monitor Caching"),
      m_dataPrepTimer("Event", "Data Preperation"),
      m_processTime("Event", "Executing Monitors") {
    m_costData->setParent(this);
    m_nThread = Config::config().getInt(kNThread);
    m_threadFnPtr = &newEventThreaded;
    m_ratesOnly = Config::config().getIsSet(kRatesOnly);
    m_isCPUPrediction = (Bool_t) Config::config().getInt(kIsCPUPrediction);
    m_pass = 0;
  }

  /**
   * ProcessEvent destructor. Clear memory.
   */
  ProcessEvent::~ProcessEvent() {
    // Good housekeeping
    monitorIt_t _it = m_monitorCollections.begin();
    for (; _it != m_monitorCollections.end(); ++_it) {
      delete _it->second;
    }
    m_monitorCollections.clear();
  }

  /**
   * Used to enable or disable individual monitoring modes. All start out disabled.
   * Modes which were enabled but are subsequently disabled are deleted and their memory freed.
   * @see MonitorType_t
   * @param _type Enum of which monitor type to act on. Can be the wild-card kAllMonitors
   * @param _isActive Boolean which specifies if monitor is to be enabled or disabled.
   */
  void ProcessEvent::setMonitoringMode(ConfKey_t _type, Bool_t _isActive) {
    assert(_type > kMonitorBegin && _type <= kDoAllMonitor);

    // If setting all, then recurs this function
    if (_type == kDoAllMonitor) {
      for (UInt_t _t = kMonitorBegin+1; _t < kDoAllMonitor; ++_t) { // Avoid hitting kAllMonitors again
        setMonitoringMode((ConfKey_t) _t, _isActive);
      }
      return;
    }

    // Find this entry in the map (may not be present)
    // This must be done by hand as object need to be of correct derived type
    monitorNonConstIt_t _it = m_monitorCollections.find( _type );
    if ( _isActive == false ) {
      // If disabling, check if present and delete if found
      if ( _it != m_monitorCollections.end() ) {
        delete _it->second;
        _it->second = 0;
        m_monitorCollections.erase( _it );
        Info("ProcessEvent::setMonitoringMode", "Disabling monitoring Level:%s Mode:%s", getLevelStr().c_str(), Config::config().getName(_type).c_str());
      }
    } else {
      // If enabling, check if present and add if not
      if ( _type == kDoRatesMonitor
           && (    getLevelStr() == Config::config().getStr(kL2String)
                || getLevelStr() == Config::config().getStr(kEFString)) ) {
        Warning("ProcessEvent::setMonitoringMode", "Rates monitoring not yet implimented for \"old style\" L2 or EF");
        return;
      }
      if ( _it == m_monitorCollections.end() ) {
        MonitorBase* _costMonitor;
        // Create object of appropriate derived type
        switch (_type) {
        case kDoChainMonitor:
          _costMonitor = new MonitorChain( m_costData );
          break;
        case kDoChainAlgorithmMonitor:
          _costMonitor = new MonitorAlgorithmChain( m_costData );
          break;
        case kDoSequenceMonitor:
          _costMonitor = new MonitorSequence( m_costData );
          break;
        case kDoSequenceAlgorithmMonitor:
          _costMonitor = new MonitorAlgorithmSequence( m_costData );
          break;
        case kDoAlgorithmMonitor:
          _costMonitor = new MonitorAlgorithm( m_costData );
          break;
        case kDoAlgorithmClassMonitor:
          _costMonitor = new MonitorAlgorithmClass( m_costData );
          break;
        case kDoROSMonitor:
          _costMonitor = new MonitorROS( m_costData );
          break;
        case kDoROBINMonitor:
          _costMonitor = new MonitorROBIN( m_costData );
          break;
        case kDoROSAlgorithmMonitor:
          _costMonitor = new MonitorROSAlgorithm( m_costData );
          break;
        case kDoROSChainMonitor:
          _costMonitor = new MonitorROSChain( m_costData );
          break;
        case kDoFullEventMonitor:
          _costMonitor = new MonitorFullEvent( m_costData );
          break;
        case kDoROIMonitor:
          _costMonitor = new MonitorROI( m_costData );
          break;
        case kDoGlobalsMonitor:
          _costMonitor = new MonitorGlobals( m_costData );
          break;
        case kDoEventProfileMonitor:
          _costMonitor = new MonitorEventProfile( m_costData );
          break;
        case kDoRatesMonitor:
          _costMonitor = new MonitorRates( m_costData );
          break;
        case kDoRatesUpgradeMonitor:
          _costMonitor = new MonitorRatesUpgrade( m_costData );
          break;
        case kDoSliceCPUMonitor:
          _costMonitor = new MonitorSliceCPU( m_costData );
          break;
        default:
          Error("ProcessEvent::setMonitoringMode", "Unknown or unimplemented Monitor Type with enum:%i", _type );
          return;
        }
        Info("ProcessEvent::setMonitoringMode", "Enabling monitoring Level:%s Mode:%s", getLevelStr().c_str(), Config::config().getName(_type).c_str());
        _costMonitor->setLevel( getLevel() );
        m_monitorCollections.insert( monitorPair_t(_type, _costMonitor) );
      }
    }
  }

  /**
   * Static function to spawn threaded monitor execution
   * @param The monitor to execute in this thread
   * @param The weight to pass it
   */
  void ProcessEvent::newEventThreaded(MonitorBase* _monitor, Float_t _weight) {
    _monitor->newEvent(_weight);
  }

  /**
   * Call once per event, passing the cost data and the event weight. This information is fed down in turn to all
   * registered and active monitors such that they may tabulate the event details.
   * We also apply here and enhanced bias or energy extrapolation weights which affect the whole event
   * @param _costData Const reference to cost data object.
   * @param _weight Event weight.
   * @return kTRUE if at least one monitor reported as having executed on the event.
   */
  Bool_t ProcessEvent::newEvent(Float_t _weight) {

    //Check for weights from energy extrapolation
    _weight *= EnergyExtrapolation::energyExtrapolation().getEventWeight( m_costData );

    //Check for enhanced bias weights.
    if (Config::config().getInt(kDoEBWeighting) == kTRUE) {
      _weight *= TrigXMLService::trigXMLService().getEventWeight( m_costData->getEventNumber(), m_costData->getLumi(), getPass() );
    }

    // For each active monitoring type, process event
    if ( isZero(_weight) == kTRUE) return false;

    // Do any monitors want to take this event?
    m_takeEventTimer.start();
    Int_t _takeEvent = kFALSE;
    for (monitorIt_t _it = m_monitorCollections.begin(); _it != m_monitorCollections.end(); ++_it) {
      _takeEvent += _it->second->getNCollectionsToProcess();
      // Note we cannot break this loop early, all monitors need to get this call to prepare their list of collections to process
      // and perform bookkeeping
    }
    m_takeEventTimer.stop();
    if (_takeEvent == 0) return kFALSE;

    // Do buffering
    if (m_ratesOnly == kFALSE) {
      m_dataPrepTimer.start();
      m_costData->bufferEvent();
      m_dataPrepTimer.stop();

      m_cacheAlgTimer.start();
      MonitorAlgorithmCommon::getAllAlgsInEvent( getLevel(), m_costData);
      m_cacheAlgTimer.stop();
      m_cacheROSTimer.start();
      MonitorROSCommon::collateROSRequests(getLevel(), m_costData);
      m_cacheROSTimer.stop();
    }


    // Special - CPU prediction mode
    // We demand a chain which was not the costmonitor chain (or any other "misbehaving" streamers etc.) to pass physics physics chain (NOT rerun).
    // Otherwise rerun on the costmon chain will mess up the prediction
    if (m_isCPUPrediction) {
      UInt_t _chainPasses = 0;
      for (UInt_t _c = 0; _c < m_costData->getNChains(); ++_c) {
        const std::string _chainName = TrigConfInterface::getHLTNameFromChainID( m_costData->getChainID(_c), m_costData->getChainLevel(_c) );
        if (Config::config().getVecMatches(kPatternsMonitor, _chainName) == kTRUE) continue;
        _chainPasses += (Int_t) m_costData->getIsChainPassed(_c);
        if (_chainPasses > 0) break;
      }
      static const std::string _ign = "IgnoreRerun";
      if (_chainPasses == 0) Config::config().set(kIgnoreRerun, 1, _ign, kUnlocked);  //Then ignore RERUN chains in this event.
      else                   Config::config().set(kIgnoreRerun, 0, _ign, kUnlocked);  
    }
    
    if (m_nThread == 1 || m_ratesOnly == kTRUE) {
      
      // Non threaded
      m_processTime.start();
      for (monitorIt_t _it = m_monitorCollections.begin(); _it != m_monitorCollections.end(); ++_it) {
        _it->second->newEvent( _weight );
      }
      m_processTime.stop();

    } else {

      // Threaded
      std::vector<std::thread> _executing;
      std::vector<MonitorBase*> _notThreadable; // Vector of function pointers

      //m_threadTimer.start();
      //m_threadTimer.stop();

      m_processTime.start();
      monitorIt_t _it = m_monitorCollections.begin();
      Bool_t _doneAll = kFALSE;

      do {
        
        while (_executing.size() < m_nThread && _doneAll == kFALSE) { // Loop until full of threads or no more to add
          MonitorBase* _monitorInstance = _it->second;
          if (_it->second->isThreadable() == kTRUE) { 
            // Spawn a new thread to call m_threadFnPtr to run _monitorInstance with argument _weight
            _executing.push_back( std::thread( m_threadFnPtr, _monitorInstance, _weight ) );
          } else { // Otherwise we execute it later
            _notThreadable.push_back( _monitorInstance );
          }
          if (++_it == m_monitorCollections.end()) {
            _doneAll = kTRUE; // Or break if the last monitor
            break;
          }
        }

        // Wait for last thread to halt
        if (_executing.size() > 0) {
          _executing.back().join(); 
          _executing.pop_back();
        }

      } while (_doneAll == kFALSE || _executing.size() > 0);

      // Do un-threadable after. Call the functions
      for (UInt_t _m = 0; _m < _notThreadable.size(); ++_m) {
        _notThreadable.at(_m)->newEvent(_weight);
      }
      m_processTime.stop();

    }
    return kTRUE;

  }

  /**
    * Sets which pass through the input file(s) we're on down to all the monitors
    * @param _pass Number of this pass
    **/
  void ProcessEvent::setPass(UInt_t _pass) {
    for (const auto& _monitor : m_monitorCollections) _monitor.second->setPass(_pass);
    m_pass = _pass;
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
    * @see setMonitoringMode(MonitorType_t _type, Bool_t _isActive)
    * @returns Map of MonitorType_t enums to TrigMonitorBase pointers.
    */
  const monitorMap_t& ProcessEvent::getMonitors() {
    return m_monitorCollections;
  }

  /**
   * Get base class pointer to a requested monitor. This can then be cast to the appropriate type if specific
   * base class dependent functionality is required.
   * @param _type The type of monitor to retrieve.
   * @result Base-class pointer to object. Returns 0 if monitor is not present.
   */
  MonitorBase* ProcessEvent::getMonitor(ConfKey_t _type) {
    assert(_type >= kDoChainMonitor && _type < kDoAllMonitor);
    monitorIt_t _it = m_monitorCollections.find( _type );
    if ( _it != m_monitorCollections.end() ) {
      return _it->second;
    } else {
      Error("ProcessEvent::getMonitor", "Collection %s is not registered.", Config::config().getName(_type).c_str());
      return 0;
    }
  }

  /**
   * Easy way of instigating the end-of-execution output sequence. The saveOutput method is called on all
   * currently active monitors.
   */
  void ProcessEvent::saveOutput() {
    // Execute end-of-run code
    for (monitorIt_t _it = m_monitorCollections.begin(); _it != m_monitorCollections.end(); ++_it) {
      UInt_t _nCounters = _it->second->getNCounters();
      if (_nCounters == 0) {
        Info("ProcessEvent::saveOutput", "Skipping end-of-run output for %s %s monitor. Monitor did not find any data to run over.",
          getLevelStr().c_str(),
          _it->second->getName().c_str());
        continue;
      }
      Info("ProcessEvent::saveOutput", "%s %s monitor processed %i counters. Writing out counters...",
        getLevelStr().c_str(),
        _it->second->getName().c_str(),
        _nCounters );
      _it->second->saveOutput();
    }
  }

  /**
  * @param _level The HLT level of this processor to set.
  */
  void ProcessEvent::setLevel(UInt_t _level) {
    m_level = _level;
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
