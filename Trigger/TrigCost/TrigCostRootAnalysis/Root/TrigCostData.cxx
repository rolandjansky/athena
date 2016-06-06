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

// Local include(s):
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"

// D3PD Reader include(s):
#include "TrigCostD3PD/TrigCostD3PDObject.h"

// ROOT include(s):
#include <TTree.h>

namespace TrigCostRootAnalysis {

  /**
   * @file TrigCostDaa.cxx
   * Useful derived information, which is not stored directly in the D3PD is calculated upon first request
   * and cached for the event. As this is the only 'interesting' code it is stored in TrigCostData_Calculations.cxx
   * The code in this source file is all boilerplate.
   * @see TrigCostData_Calculations.cxx
   */

  /**
   * TrigCostData constructor. This class provides a wrapper around the D3PDReader interface to the actual
   * underlying ROOT files which store the data. Upon creation it creates a D3PDReader instance and connects
   * to the D3PD.
   *
   * It is the only class which depends on the TrigCostD3PD package.
   *
   * It provides user-friendly access functions for all stored data types, abstracting away that the information
   * is stored in a series of nested std::vector objects.
   *
   * @see setup(const Long64_t& _master, const char* _prefix, TTree* _tree)
   * @param _master Reference to master event counter. This always holds the currently loaded event in the TTree.
   * @param _prefix D3PDMaker prefix given to all branches.
   * @param _tree Pointer to the TTree containing the data branches.
   */
  TrigCostData::TrigCostData(const Long64_t& _master, const char* _prefix, TTree* _tree) :
    m_parent(0),
    m_bufferEventNumber(-1),
    m_hasLumiData(kTRUE),
    m_emptySet(),
    m_trigCostObject(0)
  {
    setup(_master, _prefix, _tree);
  }
  
  /**
   * TrigCostData constructor. Empty constructor. Must subsiquently call setup()
   * @see TrigCostData(const Long64_t& _master, const char* _prefix, TTree* _tree)
   * @see setup(const Long64_t& _master, const char* _prefix, TTree* _tree)
   */
  TrigCostData::TrigCostData() :
    m_parent(0),
    m_bufferEventNumber(-1),
    m_hasLumiData(kTRUE),
    m_emptySet(),
    m_rosMatching(kFALSE),
    m_trigCostObject(0)
  {
    //Nothing here
  }
  
  /**
   * TrigCostData destructor, remove wrapped D3PDReader object.
   */
  TrigCostData::~TrigCostData() {
    delete m_trigCostObject;
  }

  /**
   * Setup underlying D3PDReader. Detailed description in costructor.
   * @see TrigCostData(const Long64_t& _master, const char* _prefix, TTree* _tree)
   */
  void TrigCostData::setup(const Long64_t& _master, const char* _prefix, TTree* _tree) {
    // Very important: _master is pass by ref, used directly
    m_trigCostObject = new D3PDReader::TrigCostD3PDObject(_master, _prefix);
    m_trigCostObject->ReadFrom( _tree );
#ifdef MTHREAD
    m_trigCostObject->SetActive();
#endif
    m_rosMatching = (Bool_t) Config::config().getInt(kEnableROSToAlgMatching);
    Info("TrigCostData::setup", "ROS-to-ALG matching is %s", (m_rosMatching ? "ON" : "OFF"));
  }

  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Float version of algorithm start time.
   */
  Float_t TrigCostData::getSeqAlgTimeStart(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    Float_t _r = ( m_trigCostObject->seq_alg_timeStartMicroSec()->at(_n).at(_a)/1e6 + m_trigCostObject->seq_alg_timeStartSec()->at(_n).at(_a) ) ;
    MUTEX_OFF
    return _r;
  }

  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Float version of algorithm start time.
   */
  Float_t TrigCostData::getSeqAlgTimeStop(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    Float_t _r = (  m_trigCostObject->seq_alg_timeStopMicroSec()->at(_n).at(_a)/1e6 + m_trigCostObject->seq_alg_timeStopSec()->at(_n).at(_a) ) ;
    MUTEX_OFF
    return _r;
  }
  
  /**
   * @return The event number.
   */
  Int_t TrigCostData::getEventNumber() const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->eventNumber();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return The enhanced bias weight from the ntuple - note this needs to be calculated explicitly when creating the D3PD.
   * Normally this will be done centally and distributed in this package.
   */
  Float_t TrigCostData::getEBWeight() const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->ebWeight();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return The bunch group associated with the EB weight - note this needs to be calculated explicitly when creating the D3PD.
   * This is decoded internally, but for reference matches up with Bunch Group Set 489
   * (FILLED=1, CALREQ=2, EMPTY=3, UNPAIRED_ISO=4, UNPAIRED_NONISO=5, FIRSTEMPTY=6, UNPAIRED=7)
   */
  UInt_t TrigCostData::getEBWeightBG() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->ebWeightBG();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return If - when writing the EB weighting file - this event, and hence this weight, corresponded to an unbiased L1 trigger
   */
  Bool_t TrigCostData::getEBUnbiased() const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->ebUnbiased();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return If this event was a full monitoring event or not. Only full monitoring events have 
   */
  Bool_t TrigCostData::getIsMonitoringEvent() const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->ranScaleTools();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return The event number.
   */
  Int_t TrigCostData::getRunNumber() const {
    MUTEX_ON
    Int_t _R =  m_trigCostObject ? m_trigCostObject->runNumber() : 0;
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @return The lumi block number.
   */
  Int_t TrigCostData::getLumi() const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->lumi();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return The lumi block length in seconds.
   */
  Float_t TrigCostData::getLumiLength() const {
    if (m_hasLumiData) {
      MUTEX_ON
      Float_t _lbl = m_trigCostObject->lumiLength();
      MUTEX_OFF
      if (isZero(_lbl)) {
        m_hasLumiData = kFALSE;
        return Config::config().getInt(kDefaultLBLength); 
      }
      return _lbl;
    }
    return Config::config().getInt(kDefaultLBLength);
  } 

  /**
   * @return The hash ID of the HLT processing unit which ran the execution of this event
   */
  UInt_t TrigCostData::getAppId() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->appId();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return The event number as processed by the online const monitoring (starts from 0)
   */
  Float_t TrigCostData::getCostEvent() const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->costEvent();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return Time TrigCost processed this event in seconds
   */
  UInt_t TrigCostData::getCostRunSec() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->costRunSec();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return Time TrigCost processed this event in nanoseconds
   */
  UInt_t TrigCostData::getCostRunNsec() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->costRunNsec();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return Time taken for TrigCost to process this event
   */
  Float_t TrigCostData::getTimerTrigCost() const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->timerTrigCost();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return Time to end of steering for this event
   */
  Float_t TrigCostData::getTimerEndSteer() const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->timerEndSteer();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return Time to process chains in this event
   */
  Float_t TrigCostData::getTimerChainProcessed() const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->timerChainProcess();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return Time to run the ResultBUilder in this event
   */
  Float_t TrigCostData::getTimerResultBuilder() const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->timerResultBuilder();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return Time spent in monitoring tool(s) in this event
   */
  Float_t TrigCostData::getTimerMonitoring() const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->timerMon();
    MUTEX_OFF
    return _R;
  }

  /**
   * @return The number of chains in the event.
   */
  UInt_t TrigCostData::getNChains() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->chain_n();
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return The ID of the chain.
   */
  Int_t TrigCostData::getChainID(UInt_t _n) const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->chain_counter()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return Numeric level of this chain (1, 2, 3).
   */
  Int_t TrigCostData::getChainLevel(UInt_t _n) const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->chain_level()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain passed the physics trigger in this event.
   */
  Bool_t TrigCostData::getIsChainPassed(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_isPassed()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain passed raw (before any vetoes).
   */
  Bool_t TrigCostData::getIsChainPassedRaw(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_isPassedRaw()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain was accepted as passthrough.
   */
  Bool_t TrigCostData::getIsChainPassthrough(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_isPassthrough()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain was resurrected from re-running the HLT.
   */
  Bool_t TrigCostData::getIsChainResurrected(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_isResurrected()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain was prescaled. Use the trigger configuration classes to return the prescale value.
   */
  Bool_t TrigCostData::getIsChainPrescaled(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_isPrescaled()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain was in the express stream.
   */
  Bool_t TrigCostData::getIsChainExpressStream(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_isExpressStream()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain's L1 seeding item passed L1 prescales and deadtime requirements.
   */
  Bool_t TrigCostData::getIfChainWasL1AfterVeto(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_wasL1AfterVeto()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain's L1 seeding item passed before L1 prescales were applied.
   */
  Bool_t TrigCostData::getIfChainWasL1BeforePrescale(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_wasL1BeforePrescale()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Chain index in D3PD.
   * @return If the chain's L1 seeding item passed after L1 prescale was applied.
   */
  Bool_t TrigCostData::getIfChainWasL1AfterPrescale(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->chain_wasL1AfterPrescale()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * Get time taken to execute chain as stored by the chain itself. This may not be filled.
   * @see getChainTimerFromSequences(UInt_t _n)
   * @param _n Chain index in D3PD.
   * @return Time taken to execute the chain.
   */
  Float_t TrigCostData::getChainTimer(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->chain_timer()->at(_n);
    MUTEX_OFF
    return _R;
  }
    
  /**
   * @return number of L1 items.
   */
  UInt_t TrigCostData::getNL1() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->l1_n();
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n L1 chain index in D3PD.
   * @return CTP ID number for L1 chain.
   */
  UInt_t TrigCostData::getL1CtpId(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->l1_ctpId()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n L1 chain index in D3PD.
   * @return If chain was prescale (prescale != 1).
   */
  Bool_t TrigCostData::getIsL1Prescaled(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->l1_isPrescaled()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n L1 chain index in D3PD.
   * @return If chain was vetoed (due to deadtime).
   */
  Bool_t TrigCostData::getIsL1Vetoed(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->l1_isVetoed()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n L1 chain index in D3PD.
   * @return If L1 chain passed (caused a L1 accept)
   */
  Bool_t TrigCostData::getIsL1Passed(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->l1_passed()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n L1 chain index in D3PD.
   * @return If chain passed after the application of the L1 prescale.
   */
  Bool_t TrigCostData::getIsL1PassedAfterPrescale(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->l1_passedAfterPrescale()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n L1 chain index in D3PD.
   * @return If chain passed before L1 prescale was applied.
   */
  Bool_t TrigCostData::getIsL1PassedBeforePrescale(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->l1_passedBeforePrescale()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n L1 chain index in D3PD.
   * @return If the chain passed after the L1 veto (deadtime) was applied.
   */
  Bool_t TrigCostData::getIsL1PassedAfterVeto (UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->l1_passedAfterVeto()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @return Number of sequences in the event.
   */
  UInt_t TrigCostData::getNSequences() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->seq_n();
    MUTEX_OFF
    return _R;
  }

  /**
   * Note here the slight clash of nomenclature, the return is the HLT index. It is not related to the D3PD structure.
   * @param _n Sequence index in D3PD.
   * @return The sequence index ID.
   */
  Int_t TrigCostData::getSequenceIndex(UInt_t _n) const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->seq_index()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @return The sequence's parent chain ID.
   */
  Int_t TrigCostData::getSequenceChannelCounter(UInt_t _n) const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->seq_channelCounter()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @return Numeric level of this sequence (2 or 3).
   */
  Int_t TrigCostData::getSequenceLevel(UInt_t _n) const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->seq_level()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @return The total time of sequence execution based on the algorithms in the sequence.
   */
  Float_t TrigCostData::getSequenceAlgTotalTime(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->seq_algTotalTime()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * This direct fetch may not be filled.
   * @see getSequenceAlgTotalTime(UInt_t _n)
   * @param _n Sequence index in D3PD.
   * @return The total time of sequence execution.
   */
  Float_t TrigCostData::getSequenceTime(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->seq_timer()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * Get if this sequence Trigger Element was cached.
   * @param _n Sequence index in D3PD.
   * @return If the sequence was already executed in the event.
   */
  Bool_t TrigCostData::getIsSequenceAlreadyExecuted(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->seq_isAlreadyExecuted()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @return If the sequence's algorithms were executed for its parent chain in this event.
   */
  Bool_t  TrigCostData::getIsSequenceExecuted(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->seq_isExecuted()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @return If was initialisation of sequence execution.
   */
  Bool_t TrigCostData::getIsSequenceInitial(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->seq_isInitial()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @return If sequence was executed by other sequence.
   */
  Bool_t  TrigCostData::getIsSequencePrevious(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->seq_isPrevious()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @return Number of algorithms in this sequence.
   */
  UInt_t TrigCostData::getNSeqAlgs(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->seq_alg_n()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Time of algorithm execution.
   */
  Float_t TrigCostData::getSeqAlgTimer(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->seq_alg_timer()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }

  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Algorithm start time seconds  (1 hour range, 0-3599).
   */
  UInt_t TrigCostData::getSeqAlgTimeStartSec(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->seq_alg_timeStartSec()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Algorithm start time micro seconds .
   */
  UInt_t TrigCostData::getSeqAlgTimeStartMicroSec(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->seq_alg_timeStartMicroSec()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Algorithm stop time seconds (1 hour range, 0-3599).
   */
  UInt_t TrigCostData::getSeqAlgTimeStopSec(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->seq_alg_timeStopSec()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Algorithm stop time micro seconds .
   */
  UInt_t TrigCostData::getSeqAlgTimeStopMicroSec(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->seq_alg_timeStopMicroSec()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }


  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Position of algorithm in sequence execution.
   */
  Int_t TrigCostData::getSeqAlgPosition(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    Int_t _R = (Int_t) m_trigCostObject->seq_alg_position()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return Number of algorithm attached regions of interest.
   */
  Int_t TrigCostData::getSeqAlgNRoI(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    Int_t _R = (Int_t) m_trigCostObject->seq_alg_roi_n()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return If the algorithm returned cached result.
   */
  Bool_t TrigCostData::getSeqAlgIsCached(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->seq_alg_isCached()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n Sequence index in D3PD.
   * @param _a Algorithm index in sequence.
   * @return If the algorithm had to execute to return its result.
   */
  Bool_t TrigCostData::getSeqAlgIsCalled(UInt_t _n, UInt_t _a) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->seq_alg_isCalled()->at(_n).at(_a);
    MUTEX_OFF
    return _R;
  }

  /**
   * @return Number of read out buffer entries in event.
   */
  UInt_t TrigCostData::getNROBs() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_n();
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @return Hash of requesting algorithm's name.
   */
  UInt_t TrigCostData::getROBReqID(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_requestorId()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @return Time for total ROB data fetch execution.
   */
  Float_t TrigCostData::getROBTimer(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->rob_timer()->at(_n);
    MUTEX_OFF
    return _R;
  }

  /**
   * @param _n ROB index in D3PD.
   * @return Second of request start (1 hour range, 0-3599)
   */
  UInt_t TrigCostData::getROBTimeStartSec(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_timeStartSec()->at(_n); 
    MUTEX_OFF
    return _R;
  }

  /**
   * @param _n ROB index in D3PD.
   * @return Micro second of request start.
   */  
  UInt_t TrigCostData::getROBTimeStartMicroSec(UInt_t _n) const { 
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_timeStartMicroSec()->at(_n); 
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @return Second of request stop (1 hour range, 0-3599)
   */
  UInt_t TrigCostData::getROBTimeStopSec(UInt_t _n) const { 
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_timeStopSec()->at(_n); 
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @return Micro second of request stop.
   */  
  UInt_t TrigCostData::getROBTimeStopMicroSec(UInt_t _n) const { 
    MUTEX_ON;
    UInt_t _R = m_trigCostObject->rob_timeStopMicroSec()->at(_n);
    MUTEX_OFF
    return _R; 
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @return Number of individual data requests.
   */
  UInt_t TrigCostData::getROBDataN(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_data_n()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return ROB source ID.
   */
  UInt_t TrigCostData::getROBDataID(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_data_id()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * Get size of this ROB data in kilobytes.
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return ROB data size in kb.
   */
  Float_t TrigCostData::getROBDataSize(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->rob_data_size()->at(_n).at(_r) / 1024.;
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return If the request was cached.
   */
  Bool_t TrigCostData::getIsROBDataCached(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_data_isCached()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return If the ROB was disabled in OKS and therefore not retrieved.
   */
  Bool_t TrigCostData::getIsROBDataDisabled(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_data_isDisabled()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return If the ROB was on the "ignore list" and therefore not retrieved.
   */
  Bool_t TrigCostData::getIsROBDataIgnored(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_data_isIgnored()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return If the ROB was retrieved from ROS by DataCollector.
   */
  Bool_t TrigCostData::getIsROBDataRetrieved(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_data_isRetrieved()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return If ROB is status OK.
   */
  Bool_t TrigCostData::getIsROBDataStatusOK(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_data_isStatusOk()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return If ROB is status pre-fetched.
   */
  Bool_t TrigCostData::getIsROBDataStatusPrefetched(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_data_isStatusPrefetched()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB Data index in ROB.
   * @return If ROB was requested but never arrived at processor. History unknown.
   */
  Bool_t TrigCostData::getIsROBDataUnclassified(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_data_isUnclassified()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }

  /**
   * @param _n ROB index in D3PD.
   * @return Number of ROB-sum data requests.
   */
  UInt_t TrigCostData::getROBSumN(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_sum_n()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB SumData index in ROB.
   * @return ROB Subdetector ID.
   */
  UInt_t TrigCostData::getROBSumDetID(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_sum_subDet()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB SumData index in ROB.
   * @return Number of ROBs in the sum.
   */
  UInt_t TrigCostData::getROBSumNROBs(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->rob_sum_nRob()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * Get size of this ROB data in kilobytes.
   * @param _n ROB index in D3PD.
   * @param _r ROB SumData index in ROB.
   * @return Size of ROBs in the sum, in kb.
   */
  Float_t TrigCostData::getROBSumSize(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->rob_sum_size()->at(_n).at(_r) / 1024.;
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB SumData index in ROB.
   * @return If the request was cached.
   */
  Bool_t TrigCostData::getIsROBSumCached(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_sum_isCached()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB SumData index in ROB.
   * @return If the ROB was disabled in OKS and therefore not retrieved.
   */
  Bool_t TrigCostData::getIsROBSumDisabled(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_sum_isDisabled()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB SumData index in ROB.
   * @return If the ROB was on the "ignore list" and therefore not retrieved.
   */
  Bool_t TrigCostData::getIsROBSumIgnored(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_sum_isIgnored()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB SumData index in ROB.
   * @return If the ROB was retrieved from ROS by DataCollector
   */
  Bool_t TrigCostData::getIsROBSumRetrieved(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_sum_isRetrieved()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n ROB index in D3PD.
   * @param _r ROB SumData index in ROB.
   * @return If ROB was requested but never arrived at processor. History unknown.
   */
  Bool_t TrigCostData::getIsROBSumUnclassified(UInt_t _n, UInt_t _r) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->rob_sum_isUnclassified()->at(_n).at(_r);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @return Number of Regions of Interest in the event.
   */
  UInt_t TrigCostData::getNRoIs() const  {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->roi_n();
    MUTEX_OFF
    return _R;
  }
 
  /**
   * @param _n RoI index in D3PD.
   * @return Region of Interest ID number.
   */
  Int_t TrigCostData::getRoIID(Int_t _n) const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->roi_id()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return Region of Interest area in (eta,phi) space.
   */
  Float_t TrigCostData::getRoIArea(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->roi_area()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return Region of Interest pseudorapidity.
   */
  Float_t TrigCostData::getRoIEta(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->roi_eta()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return Region of Interest azimuthal angle.
   */
  Float_t TrigCostData::getRoIPhi(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->roi_phi()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return Number of passed L1 thresholds for Region of Interest.
   */
  Int_t TrigCostData::getRoINL1Thresh(UInt_t _n) const {
    MUTEX_ON
    Int_t _R = (Int_t) m_trigCostObject->roi_nL1Thresholds()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return If Region of Interest is of type electromagnetic / tau.
   */
  Bool_t TrigCostData::getIsRoIEmTau(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_isTypeEmTau()->at(_n);
    MUTEX_OFF
    return _R;
  }

  /**
   * @param _n RoI index in D3PD.
   * @return Disambiguate EM and tau
   */
  Bool_t TrigCostData::getIsRoITau(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_isTau()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return If Region of Interest is of type energy (e.g. total, missing)
   */
  Bool_t TrigCostData::getIsRoIEnergy(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_isTypeEnergy()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return If Region of Interest is of type jet.
   */
  Bool_t TrigCostData::getIsRoIJet(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_isTypeJet()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return If Region of Interest is of type jet ET.
   */
  Bool_t TrigCostData::getIsRoIJetEt(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_isTypeJetEt()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return If Region of Interest is of type muon.
   */
  Bool_t TrigCostData::getIsRoIMuon(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_isTypeMuon()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n RoI index in D3PD.
   * @return If Region of Interest is of type none.
   */
  Bool_t TrigCostData::getIsRoINone(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_isTypeNone()->at(_n);
    MUTEX_OFF
    return _R;
  }  

  /**
   * @param _n RoI index in D3PD.
   * @return ET or pT or total ET sum (depends on RoI type).
   */
  Float_t TrigCostData::getRoIEt(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->roi_et()->at(_n);
    MUTEX_OFF
    return _R;
  }

  Float_t TrigCostData::getRoIEtLarge(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->roi_etLarge()->at(_n);
    MUTEX_OFF
    return _R;
  }

  Int_t   TrigCostData::getRoIMuonCharge(UInt_t _n) const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->roi_muCharge()->at(_n);
    MUTEX_OFF
    return _R;
  }

  Int_t   TrigCostData::getRoIEmTauIsoBits(UInt_t _n) const {
    MUTEX_ON
    Int_t _R = m_trigCostObject->roi_isoBits()->at(_n);
    MUTEX_OFF
    return _R;
  }

  Float_t TrigCostData::getRoIVectorEX(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->roi_vectorEX()->at(_n);
    MUTEX_OFF
    return _R;
  }

  Float_t TrigCostData::getRoIVectorEY(UInt_t _n) const {
    MUTEX_ON
    Float_t _R = m_trigCostObject->roi_vectorEY()->at(_n);
    MUTEX_OFF
    return _R;
  }

  Bool_t  TrigCostData::getRoIOverflowEX(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_overflowEX()->at(_n);
    MUTEX_OFF
    return _R;
  }

  Bool_t  TrigCostData::getRoIOverflowEY(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_overflowEY()->at(_n);
    MUTEX_OFF
    return _R;
  }

  Bool_t  TrigCostData::getRoIOverflowET(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->roi_overflowET()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @return Number of trigger elements in event.
   */
  UInt_t TrigCostData::getNTEs() const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_n();
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return Trigger element index.
   */
  UInt_t TrigCostData::getTEIndex(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_index()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return Trigger element ID number.
   */
  UInt_t TrigCostData::getTEID(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_id()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @param _c Child index in TE.
   * @return Trigger element index requested of child (successor).
   */
  UInt_t TrigCostData::getTEChildIndex(UInt_t _n, UInt_t _c) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_childIndex()->at(_n).at(_c);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @param _c Parent index in TE.
   * @return Trigger element index of requested parent (predecessor).
   */
  UInt_t TrigCostData::getTEParentIndex(UInt_t _n, UInt_t _c) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_parentIndex()->at(_n).at(_c);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @param _c CLID index in TE.
   * @return CLID of requested attached feature.
   */
  UInt_t TrigCostData::getTECLIDIndex(UInt_t _n, UInt_t _c) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_clid()->at(_n).at(_c);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @param _c RoI index in TE.
   * @return RoI ID of requested seeding RoI
   */
  UInt_t TrigCostData::getTERoIIDIndex(UInt_t _n, UInt_t _c) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_roiId()->at(_n).at(_c);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return Number of parent TEs.
   */
  UInt_t TrigCostData::getTEChildSize(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_childIndex()->at(_n).size();
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return Number of child TEs.
   */
  UInt_t TrigCostData::getTEParentSize(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_parentIndex()->at(_n).size();
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return Number of attached CLIDs.
   */
  UInt_t TrigCostData::getTECLIDSize(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_clid()->at(_n).size();
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return Number of seeding RoIs.
   */
  UInt_t TrigCostData::getTERoIIDSize(UInt_t _n) const {
    MUTEX_ON
    UInt_t _R = m_trigCostObject->te_roiId()->at(_n).size();
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If TE was of Active state.
   */
  Bool_t TrigCostData::getIsTEActiveState(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isActiveState()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If TE was of Error state.
   */
  Bool_t TrigCostData::getIsTEErrorState(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isErrorState()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If was initial TE.
   */
  Bool_t TrigCostData::getIsTEInitial(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isInitialTe()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If is L1 threshold TE attached to RoI TE.
   */
  Bool_t TrigCostData::getIsTEL1Threshold(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isL1ThresholdTe()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If TE is output node of passed or passedRaw chain in EF.
   */
  Bool_t TrigCostData::getIsTEOutputEFNode(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isOutputEFNode()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If TE is output node of passed or passedRaw chain in L2.
   */
  Bool_t TrigCostData::getIsTEOutputL2Node(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isOutputL2Node()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If TE is regular TE in navigation tree.
   */
  Bool_t TrigCostData::getIsTERegularTe(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isRegularTe()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If TE is RoI TE attached to initial TE.
   */
  Bool_t TrigCostData::getIsTERoITe(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isRoITe()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If TE is terminal node, with no more TEs seeded from it.
   */
  Bool_t TrigCostData::getIsTETerminalNode(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isTerminalNode()->at(_n);
    MUTEX_OFF
    return _R;
  }
  
  /**
   * @param _n TE index in D3PD.
   * @return If TE is TopologicalTE (and therefore has > 1 parent)
   */
  Bool_t TrigCostData::getIsTETopologicalTe(UInt_t _n) const {
    MUTEX_ON
    Bool_t _R = (Bool_t) m_trigCostObject->te_isTopologicalTe()->at(_n);
    MUTEX_OFF
    return _R;
  }

  /**
   * Pointer to the process event object, for debug mostly
   */
  ProcessEvent* TrigCostData::getParent() const { 
    return m_parent;
  }
  
  /**
   * Pointer to the process event object, for debug mostly
   */
  void TrigCostData::setParent(ProcessEvent* _parent) const { 
    m_parent = _parent;
  }
  
} // namespace TrigCostRootAnalysis
