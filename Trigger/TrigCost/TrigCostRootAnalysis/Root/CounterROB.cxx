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
#include "../TrigCostRootAnalysis/MonitorROSCommon.h"
#include "../TrigCostRootAnalysis/CounterROB.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

namespace TrigCostRootAnalysis {

  /**
   * Read Out Buffer counter constructor. Sets values of internal variables and sets up data store.
   * @param _name Const ref to name of Read Out Buffer
   * @param _ID ID number of Read Out Buffer
   */
  CounterROB::CounterROB( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel, MonitorBase* _parent )
    : CounterBase(_costData, _name, _ID, _detailLevel, _parent) {

    if (m_detailLevel == 0) m_dataStore.setHistogramming(kFALSE);

    m_dataStore.newVariable(kVarEventsActive).setSavePerEvent();

    m_dataStore.newVariable(kVarCalls)
    .setSavePerEvent("Number Of Calls Per Event;Number of Calls;Events");

    m_dataStore.newVariable(kVarTime)
    .setSavePerCall("Time Per Call;Time [ms];Calls")
    .setSavePerEvent("Time Per Event;Time [ms];Events");

    m_dataStore.newVariable(kVarROBRets)
    .setSavePerCall()
    .setSavePerEvent("Number Of Retrieved ROBs Per Event;Retrieved ROBs;Events");

    m_dataStore.newVariable(kVarROBRetSize)
    .setSavePerCall("Size Of Retrieved ROBs Per Call;Retrieved ROBs Size [kB];Calls")
    .setSavePerEvent("Size Of Retrieved ROBs Per Event;Retrieved ROBs Size [kB];Events");

    m_dataStore.newVariable(kVarROBReqs)
    .setSavePerCall()
    .setSavePerEvent("Number Of Cached ROBs Per Event;Cached ROBs;Events");

    m_dataStore.newVariable(kVarROBReqSize)
    .setSavePerCall("Size Of Cached ROBs Per Call;Cached ROBs Size [kB];Calls")
    .setSavePerEvent("Size Of Cached ROBs Per Event;Cached ROBs Size [kB];Events");

    m_dataStore.newVariable(kVarROBDisabled).setSavePerEvent();
    m_dataStore.newVariable(kVarROBIgnored).setSavePerEvent();
    m_dataStore.newVariable(kVarROBNotOK).setSavePerEvent();
    m_dataStore.newVariable(kVarROBUnclassified).setSavePerEvent();
    m_dataStore.newVariable(kVarROBPrefetched).setSavePerEvent();


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

    _weight *= getPrescaleFactor( _e );
    if (isZero(_weight) == kTRUE) return;

   
    Bool_t _isBad = kFALSE;
    
    IntSet_t* _ROBsForCounter(nullptr);

    // Collect data to fill. This depends on if we are looking at a ROBIN or a ROS

    /*
    switch (getStrDecoration(kDecType)) {

        case Config::config().getStr(kROSString):    //Monitoring a ROS, collate all ROBINs in request
            MonitorROSCommon* _parent = (MonitorROSCommon*) getParent();;
            _ROBINs = _parent->getROSMapping(_e).find( getStrDecoration(kDecMyROS) )->second;
            break;

        case Config::config().getStr(kROBINString):  //Monitoring a ROBIN, collate all ROBs in request
            MonitorROSCommon* _ROBINparent = (MontiorROSCommon*) getParent();
            _ROBINs = _ROBINparent->getROBINMapping(_e).find( getStrDecoration(kDecMyROBIN) )->second;
            break;

        default:
            _ROBINs.insert( _f);

	    } */

    if (getStrDecoration(kDecType) == Config::config().getStr(kROSString)) {
      // I'm monitoring a ROS. I need to collate all my ROBINs in this request
      MonitorROSCommon* _parent = (MonitorROSCommon*) getParent(); // Up-cast
      _ROBsForCounter =&( _parent->getROSMapping(_e).find( getStrDecoration(kDecMyROS) )->second); // Get my mapping
    } 
    
    else if (getStrDecoration(kDecType) == Config::config().getStr(kROBINString)) {
      //Monitoring a ROBIN, need to collate all ROBS
      MonitorROSCommon* _ROBINparent = (MonitorROSCommon*) getParent();
      _ROBsForCounter = &(_ROBINparent->getROBINMapping(_e).find( getStrDecoration(kDecMyROS) )->second); //Get ROBIN mapping
    }

    else {
      // Else we're just monitoring a ROBIN hence it's only me!
      //_ROBsForCounter.insert( _f );
      Error("CounterROB::processEventCounter", "Does not match ROS or ROBIN");
    }

    // Put nullptr check here so covertiry sees it
    if (_ROBsForCounter == nullptr) return;

    Int_t _nROBSForCounter = _ROBsForCounter->size();

    // Loop over the ROBINs in this ROS request
    for (IntSetIt_t _it = _ROBsForCounter->begin(); _it != _ROBsForCounter->end(); ++_it) {
      UInt_t _ROBIndex = (*_it);

      /*
      if (m_costData->getIsROBDataRetrieved(_e, _ROBIndex)) {
        _retSize += m_costData->getROBDataSize(_e, _ROBIndex);
        _isRet = kTRUE;
	} */

      m_dataStore.store(kVarROBRets, m_costData->getIsROBDataRetrieved(_e, _ROBIndex), _weight);
      if (m_costData->getIsROBDataRetrieved(_e, _ROBIndex)) m_dataStore.store(kVarROBRetSize, m_costData->getROBDataSize(_e, _ROBIndex) , _weight);

      /*
      if (m_costData->getIsROBDataCached(_e, _ROBIndex)) {
        _reqSize += m_costData->getROBDataSize(_e, _ROBIndex);
        _isReq = kTRUE;
      }
      */

      m_dataStore.store(kVarROBReqs, m_costData->getIsROBDataCached(_e, _ROBIndex) , _weight);
      if (m_costData->getIsROBDataCached(_e, _ROBIndex) ) m_dataStore.store(kVarROBReqSize, m_costData->getROBDataSize(_e, _ROBIndex), _weight);

      m_dataStore.store(kVarROBUnclassified, m_costData->getIsROBDataUnclassified(_e, _ROBIndex), _weight);
      m_dataStore.store(kVarROBPrefetched, m_costData->getIsROBDataStatusPrefetched(_e, _ROBIndex), _weight);
      m_dataStore.store(kVarROBIgnored, m_costData->getIsROBDataIgnored(_e, _ROBIndex), _weight);
      m_dataStore.store(kVarROBDisabled, m_costData->getIsROBDataDisabled(_e, _ROBIndex), _weight);
			
			/*
      if (m_costData->getIsROBDataUnclassified(_e, _ROBIndex)) _isUnclassified = kTRUE;
      if (m_costData->getIsROBDataIgnored(_e, _ROBIndex)) _isIgnored = kTRUE;
      if (m_costData->getIsROBDataStatusOK(_e, _ROBIndex)) == kFALSE) _isBad = kTRUE;
      if (m_costData->getIsROBDataDisabled(_e, _ROBIndex)) _isDisabled = kTRUE;
      if (m_costData->getIsROBDataStatusPrefetched(_e, _ROBIndex)) _isPrefetched = kTRUE;

			*/

      if (m_costData->getIsROBDataStatusOK(_e, _ROBIndex) == kFALSE) _isBad = kTRUE;
      m_dataStore.store(kVarROBNotOK, (int)_isBad, _weight);
      if (_isBad == kTRUE && Config::config().getDisplayMsg(kMsgBadROB) == kTRUE) {
        Warning("CounterROB::processEventCounter","ROB data from %s (ID 0x%x) is flagged as NOT OK.", getName().c_str(), m_costData->getROBDataID(_e, _ROBIndex));
      }
    }

    m_dataStore.store(kVarCalls, 1, _weight);

    // We divide this time evenly to all ROBINs which were part of this request
    Float_t _rosReqTime = m_costData->getROBTimer(_e); // Total time
    _rosReqTime /= (Float_t) m_costData->getROBDataN( _e ); // Time per ROBIN
    _rosReqTime *= _nROBSForCounter; // ROBINs in this request
    m_dataStore.store(kVarTime, _rosReqTime, _weight);

    /*
    if ( _isRet ) {
      m_dataStore.store(kVarROBRets, 1., _weight);
      m_dataStore.store(kVarROBRetSize, _retSize, _weight);
    }
    if ( _isReq ) {
      m_dataStore.store(kVarROBReqs, 1., _weight);
      m_dataStore.store(kVarROBReqSize, _reqSize, _weight);
    }

    if ( _isBad )  m_dataStore.store(kVarROBNotOK, 1., _weight);
    if ( _isUnclassified )  m_dataStore.store(kVarROBUnclassified, 1., _weight);
    if ( _isPrefetched )  m_dataStore.store(kVarROBPrefetched, 1., _weight);
    if ( _isIgnored )  m_dataStore.store(kVarROBIgnored, 1., _weight);
    if ( _isDisabled )  m_dataStore.store(kVarROBDisabled, 1., _weight);
    */

    if ( Config::config().debug() ) debug(_e, _f);

  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterROB::endEvent(Float_t _weight) {
    m_dataStore.store(kVarEventsActive, 1., _weight);
    m_dataStore.endEvent();
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * We associate ROS with Alg, then get the alg's chain, then get the weighting factor for the chain
   * @return Multiplicative weighting factor
   */
  Double_t CounterROB::getPrescaleFactor(UInt_t _e) {
    Int_t _seqID = m_costData->getROBAlgLocation(_e).first;
    if (_seqID < 0) return 1.; // Could not find associated alg - no additonal scaling
    return TrigXMLService::trigXMLService().getHLTCostWeightingFactor(
      TrigConfInterface::getHLTNameFromChainID( m_costData->getSequenceChannelCounter(_seqID),
      m_costData->getSequenceLevel(_seqID) ) );
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
