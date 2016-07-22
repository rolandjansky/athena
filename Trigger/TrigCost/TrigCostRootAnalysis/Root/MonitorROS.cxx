// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <utility> //std::pair
#include <iostream>
#include <assert.h>

//Local include(s):
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/MonitorROS.h"
#include "../TrigCostRootAnalysis/CounterROB.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/ROSConfService.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT includes
#include <TError.h>

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorROS::MonitorROS(const TrigCostData* _costData) : MonitorBase(_costData, "ROS") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    allowSameIDCounters();
  }

  /**
   * Process new event for this monitor.
   * For the ROS, all ROS data is looped over and recorded.
   * @param _weight The event weight.
   */
  void MonitorROS::newEvent(Float_t _weight ) {
    m_timer.start();
    if ( Config::config().debug() ) {
      Int_t _N = 0;
      for (UInt_t _rob = 0; _rob < m_costData->getNROBs(); ++_rob) {
        _N += m_costData->getROBDataN( _rob);
      }
      Info("MonitorROS::newEvent", "*** Processing ROS ***  Size %i ***", _N  );
    }

    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;

      startEvent();

      for (UInt_t _robReq = 0; _robReq < m_costData->getNROBs(); ++_robReq) {
        // We want to call each ROS once, but the inner loop is over ROBINs
        // We now do this mapping in advance
        StringIntSetMap_t _ROSMapping = getROSMapping(_robReq);
        for (StringIntSetMapIt_t _reqIt = _ROSMapping.begin(); _reqIt != _ROSMapping.end(); ++_reqIt) {
          // Get the counter - note we do not store any ID here
          CounterBase* _counter =  getCounter( _counterMap, (*_reqIt).first, 0 /*not used*/ );
          // This lets the counter know it should ask its parent for the full set of ROBINs to collate
          if (_counter->getCalls() == 0) {
            _counter->decorate(kDecType, Config::config().getStr(kROSString));
	    _counter->decorate(kDecMyROS, (*_reqIt).first);
            
          }
          _counter->processEventCounter( _robReq, UINT_MAX /*not used*/, _weight );
        }
      }

      endEvent(_weight);
    }
    m_timer.stop();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorROS::getIfActive(ConfKey_t _mode) {
    switch(_mode) {
      case kDoAllSummary:       return kTRUE;
      case kDoKeySummary:       return kTRUE;
      case kDoLumiBlockSummary: return kTRUE;
      default: Error("MonitorROS::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(_mode).c_str() );
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorROS::saveOutput() {

    m_filterOutput = kTRUE; // Apply any user-specified name filter to output

    VariableOptionVector_t _toSave = m_dummyCounter->getAllHistograms();
    sharedHistogramOutputRoutine( _toSave );

    std::vector<TableColumnFormatter> _toSaveTable;
    addCommonTableEntries( _toSaveTable );
    sharedTableOutputRoutine( _toSaveTable );

  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct derived type.
   */
  CounterBase* MonitorROS::newCounter(  const std::string &_name, Int_t _ID  ) {
    return new CounterROB( m_costData, _name,  _ID, m_detailLevel, (MonitorBase*)this );
  }

} // namespace TrigCostRootAnalysis
