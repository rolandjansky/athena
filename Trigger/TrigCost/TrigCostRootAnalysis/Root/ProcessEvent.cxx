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

//Local include(s):
#include "../TrigCostRootAnalysis/ProcessEvent.h"
#include "../TrigCostRootAnalysis/MonitorBase.h"
#include "../TrigCostRootAnalysis/MonitorChain.h"
#include "../TrigCostRootAnalysis/MonitorSequence.h"
#include "../TrigCostRootAnalysis/MonitorAlgorithm.h"
#include "../TrigCostRootAnalysis/MonitorAlgorithmSequence.h"
#include "../TrigCostRootAnalysis/MonitorAlgorithmChain.h"
#include "../TrigCostRootAnalysis/MonitorROS.h"
#include "../TrigCostRootAnalysis/MonitorL1ChainMap.h"
#include "../TrigCostRootAnalysis/MonitorFullEvent.h"
#include "../TrigCostRootAnalysis/MonitorROI.h"
#include "../TrigCostRootAnalysis/MonitorGlobals.h"
#include "../TrigCostRootAnalysis/MonitorRates.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"

namespace TrigCostRootAnalysis {

  /**
   * ProcessEvent constructor. Not much here.
   */
  ProcessEvent::ProcessEvent(const TrigCostData* _costData, UInt_t _level, const std::string& _name) : m_costData(_costData), m_level(_level), m_name(_name) {
    m_costData->setParent(this);
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
        // Special case, don't enable L1 chain mapping for EF
        if ((ConfKey_t) _t == kDoL1ChainMapMonitor && getLevel() == 3) {
          continue;
        }
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
      // Special case, cannot do L1ChainMap for EF processor
      if ( _type == kDoL1ChainMapMonitor && getLevel() == 3) {
        Error("ProcessEvent::setMonitoringMode", "Unable to use the L1 Chain Map Monitor in the EF, use this option only for L2 or HLT");
        return;
      }
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
        case kDoROSMonitor:
          _costMonitor = new MonitorROS( m_costData );
          break;
        case kDoL1ChainMapMonitor:
          _costMonitor = new MonitorL1ChainMap( m_costData );
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
        case kDoRatesMonitor:
          _costMonitor = new MonitorRates( m_costData );
          break;
        default:
          Error("ProcessEvent::setMonitoringMode", "Unknown or unimplemented Monitor Type with enum:%i", _type );
          return;
        }
        Info("ProcessEvent::setMonitoringMode", "Enabling monitoring Level:%i Mode:%s", getLevel(), Config::config().getName(_type).c_str());
        _costMonitor->setLevel( getLevel() );
        m_monitorCollections.insert( monitorPair_t(_type, _costMonitor) );
      }
    }
  }
  
  /**
   * Call once per event, passing the cost data and the event weight. This information is fed down in turn to all
   * registered and active monitors such that they may tabulate the event details.
   * @param _costData Const reference to cost data object.
   * @param _weight Event weight.
   */
  void ProcessEvent::newEvent(Float_t _weight) {
    // For each active monitoring type, process event
    if ( isZero(_weight) == kTRUE) return;
    for (monitorIt_t _it = m_monitorCollections.begin(); _it != m_monitorCollections.end(); ++_it) {
      _it->second->newEvent( _weight );
    }
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
      Info("ProcessEvent::saveOutput", "Doing end-of-run output for %s %s monitor. Counters:%i",
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
