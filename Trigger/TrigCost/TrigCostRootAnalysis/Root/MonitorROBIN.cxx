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
#include "../TrigCostRootAnalysis/MonitorROBIN.h"
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
  MonitorROBIN::MonitorROBIN(const TrigCostData* _costData) : MonitorBase(_costData, "ROBIN") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    allowSameIDCounters();
  }

  /**
   * Process new event for this monitor.
   * For the ROS, all ROS data is looped over and recorded.
   * @param _weight The event weight.
   */
  void MonitorROBIN::newEvent(Float_t _weight ) {
    m_timer.start();
    if ( Config::config().debug() ) {
      Int_t _N = 0;
      for (UInt_t _rob = 0; _rob < m_costData->getNROBs(); ++_rob) {
        _N += m_costData->getROBDataN( _rob);
      }
      Info("MonitorROBIN::newEvent", "*** Processing ROBIN ***  Size %i ***", _N  );
    }

    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;

      startEvent();

      // All looping over individual RODs to do their event monitoring can go here
      for (UInt_t _robReq = 0; _robReq < m_costData->getNROBs(); ++_robReq) {
        //Info("TEMP", "*** ROBIN MON *** REQUEST %i / %i", _rob,  m_costData->getNROBs()  );

	StringIntSetMap_t _ROBINMapping = getROBINMapping(_robReq);
	for (StringIntSetMapIt_t _reqIt = _ROBINMapping.begin(); _reqIt != _ROBINMapping.end(); ++_reqIt) {
	  CounterBase* _counter = getCounter( _counterMap, (*_reqIt).first, 0 /*not used*/ );
	  
	  if (_counter->getCalls() == 0) {
	    _counter->decorate(kDecType, Config::config().getStr(kROBINString));
	    _counter->decorate(kDecMyROS, (*_reqIt).first);
	    
	  }
	  
	  _counter->processEventCounter( _robReq, UINT_MAX /*not used*/, _weight );
	}
      }

      endEvent(_weight);
    }
    m_timer.stop();
  }


	/*
        for (UInt_t _robData = 0; _robData < m_costData->getROBDataN( _robReq); ++_robData) {

          Int_t _robId = m_costData->getROBDataID(_robReq, _robData);
          const std::string _robinName = ROSConfService::rosConfService().getRobinNameFromId( (UInt_t) _robId );

          // TEMP
          //const std::string _rosName = ROSConfService::rosConfService().getRosNameFromFromRobinName( _robinName );
          //Info("TEMP", "%i] %s -> %s", _robData, _rosName.c_str(), _robinName.c_str()  );



          CounterBase* _counter =  getCounter( _counterMap, _robinName, _robId );
          // This lets the counter know it should look at just this one ROBIN
          if (_counter->getCalls() == 0) _counter->decorate(kDecType, Config::config().getStr(kROBINString));
          _counter->addMultiID( (UInt_t) _robId );
          _counter->processEventCounter( _rob, _robData, _weight );
        }
      }

      endEvent(_weight);
    }
    m_timer.stop();
  }
	*/
  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorROBIN::getIfActive(ConfKey_t _mode) {
    switch(_mode) {
      case kDoAllSummary:       return kTRUE;
      case kDoKeySummary:       return kTRUE;
      case kDoLumiBlockSummary: return kFALSE;
      default: Error("MonitorROBIN::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(_mode).c_str() );
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorROBIN::saveOutput() {

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
  CounterBase* MonitorROBIN::newCounter(  const std::string &_name, Int_t _ID  ) {
    return new CounterROB( m_costData, _name,  _ID, m_detailLevel, (MonitorBase*)this );
  }

} // namespace TrigCostRootAnalysis
