// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterROB.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"

namespace TrigCostRootAnalysis {

  /**
   * Read Out Buffer counter constructor. Sets values of internal variables and sets up data store.
   * @param _name Const ref to name of Read Out Buffer
   * @param _ID ID number of Read Out Buffer
   */
  CounterROB::CounterROB( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel ) : CounterBase(_costData, _name, _ID, _detailLevel) {
    
    m_dataStore.newVariable(kVarCalls)
    .setSavePerCall()
    .setSavePerEvent("Number Of ROBs Per Event;Number of ROBs;Events");
    
    m_dataStore.newVariable(kVarTime)
    .setSavePerCall("ROS Time Per ROB;Time [ms];ROBs")
    .setSavePerEvent("ROS Time Per Event;Time [ms];Events");
    
    m_dataStore.newVariable(kVarROBRets)
    .setSavePerCall()
    .setSavePerEvent("Number Of Retrieved ROBs Per Event;Retrieved ROBs;Events");

    m_dataStore.newVariable(kVarROSCalls)
    .setSavePerCall("Total ROBs (of all types) per ROS Request;Number of ROBs;Calls");
    
    m_dataStore.newVariable(kVarROBRetSize)
    .setSavePerCall("Size Of Retrieved ROBs Per Call;Retrieved ROBs Size [kB];Calls")
    .setSavePerEvent("Size Of Retrieved ROBs Per Event;Retrieved ROBs Size [kB];Events");
    
    m_dataStore.newVariable(kVarROBReqs)
    .setSavePerCall()
    .setSavePerEvent("Number Of Cached ROBs Per Event;Cached ROBs;Events");
    
    m_dataStore.newVariable(kVarROBReqSize)
    .setSavePerCall("Size Of Cached ROBs Per Call;Cached ROBs Size [kB];Calls")
    .setSavePerEvent("Size Of Cached ROBs Per Event;Cached ROBs Size [kB];Events");

    m_dataStore.newVariable(kVarROBOther).setSavePerEvent();

  }
  
  /**
   * Counter destructor.
   */
  CounterROB::~CounterROB() {
  }
  
  /**
   * Reset per-event counter(s). Not used currently for ROB counter.
   */
  void CounterROB::startEvent() {
  }
  
  /**
   * Perform monitoring of a Read Out Buffer call within an event.
   * @param _e Read Out Buffer index in D3PD.
   * @param _f ROBIN index within ROB
   * @param _weight Event weight.
   */
  void CounterROB::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;

    if (m_costData->getIsROBDataStatusOK(_e, _f) == kFALSE && Config::config().getDisplayMsg(kMsgBadROB) == kTRUE) { 
      Warning("CounterROB::processEventCounter","ROB data from %s (ID 0x%x) is flagged as NOT OK.", getName().c_str(), m_costData->getROBDataID(_e, _f));  
    }

    m_dataStore.store(kVarCalls, 1, _weight);

    Float_t _rosReqTime = m_costData->getROBTimer(_e);
    // We divide this time through by the number of ROBs the request resulted in. TODO check with Doug
    _rosReqTime /= (Float_t) m_costData->getROBDataN( _e );
    m_dataStore.store(kVarTime, _rosReqTime, _weight); 

    m_dataStore.store(kVarROSCalls, m_costData->getROBDataN( _e ), _weight); 
    
    if (m_costData->getIsROBDataRetrieved(_e, _f)) {
      m_dataStore.store(kVarROBRets, 1., _weight);
      m_dataStore.store(kVarROBRetSize, m_costData->getROBDataSize(_e, _f), _weight);
    }
    if (m_costData->getIsROBDataCached(_e, _f)) {
      m_dataStore.store(kVarROBReqs, 1., _weight);
      m_dataStore.store(kVarROBReqSize, m_costData->getROBDataSize(_e, _f), _weight);
    }

    if (m_costData->getIsROBDataUnclassified(_e, _f) || m_costData->getIsROBDataIgnored(_e, _f) || m_costData->getIsROBDataDisabled(_e, _f)) {
      m_dataStore.store(kVarROBOther, 1., _weight);
    }

    if ( Config::config().debug() ) debug(_e, _f);
  }
  
  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterROB::endEvent() {
    m_dataStore.endEvent();
  }
  
  /**
   * Output debug information on this call to the console
   */
  void CounterROB::debug(UInt_t _e, UInt_t _f) {
    if (_f == 0) { //For first ROB in ROSReq
      Info("CounterROB::debug", "ROS %s: ReqID:%i Timer:%.2f NData:%i NSum:%i",
        getName().c_str(),
        m_costData->getROBReqID(_e),
        m_costData->getROBTimer(_e),
        m_costData->getROBDataN(_e),
        m_costData->getROBSumN(_e) );
    }
    Info("CounterROB::debug", "\t%i] ROB %s: Size:%.2f Cache:%i Disabled:%i Ignored:%i Retr.:%i Prefetched:%i Unclassified:%i OK:%i",
      _f,
      getName().c_str(),
      m_costData->getROBDataSize(_e, _f),
      m_costData->getIsROBDataCached(_e, _f),
      m_costData->getIsROBDataDisabled(_e, _f),
      m_costData->getIsROBDataIgnored(_e, _f),
      m_costData->getIsROBDataRetrieved(_e, _f),
      m_costData->getIsROBDataStatusPrefetched(_e, _f),
      m_costData->getIsROBDataUnclassified(_e, _f),
      m_costData->getIsROBDataStatusOK(_e, _f) );
  }
  
} // namespace TrigCostRootAnalysis
