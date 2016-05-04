/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/TrigCountTrtHitsHypo.h"
#include "TrigInDetEvent/TrigTrtHitCounts.h"

#include "xAODEventInfo/EventInfo.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

//-----------------------------------------------------------------------------

TrigCountTrtHitsHypo::TrigCountTrtHitsHypo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::HypoAlgo(name, pSvcLocator),
    m_log(msgSvc(), name) {
  declareProperty( "AcceptAll",                  m_acceptAll = false );                                                           
  declareProperty( "TimeOverThresholdCut",       m_timeOverThresholdCut = 18. );      
  declareProperty( "TrtHitsEndcaps",             m_trtHitsEC_cut = 60. );     
  declareProperty( "TotalTrtHits",               m_totalTrtHits_cut = -1. );
  declareMonitoredVariable("NTrtRDO",            m_nTrtHits);

  // Additional initialization for Coverity cleaning
  declareProperty( "hltExecuteInitialisationRun", m_hltExecuteInitialisationRun = true);
  declareProperty( "trt_barrel_a_side",   m_trt_barrel_a_side = true );     
  declareProperty( "trt_barrel_c_side",   m_trt_barrel_c_side = true );     
  declareProperty( "trt_endcap_a_side",   m_trt_endcap_a_side = true );     
  declareProperty( "trt_endcap_c_side",   m_trt_endcap_c_side = true );     
  declareProperty( "nTrtHits_B",          m_nTrtHits_B = 0. );     
  declareProperty( "nTrtHits_A",          m_nTrtHits_A = 0. );     
  declareProperty( "nTrtHits_C",          m_nTrtHits_C = 0. );     
  declareProperty( "maxTrtHitsEC",        m_maxTrtHitsEC = 0. );     
}

//-----------------------------------------------------------------------------

TrigCountTrtHitsHypo::~TrigCountTrtHitsHypo(){;}

//-----------------------------------------------------------------------------

HLT::ErrorCode TrigCountTrtHitsHypo::hltInitialize() {
  m_log.setLevel(outputLevel());
  if (msgLvl() <= MSG::INFO) {
    m_log << MSG::INFO << "Initialising this TrigCountTrtHitsHypo: " << name() << endreq;
    
    m_log << MSG::INFO << "================ Hypo Settings ================" << endreq;
    m_log << MSG::INFO << " AcceptAll ---------------------  " << (m_acceptAll==true ? "True" : "False") << endreq;
    m_log << MSG::INFO << " TimeOverThresholdCut  ---------  " << m_timeOverThresholdCut << endreq;
 
    m_log << MSG::INFO << " Requiring hits in ECA or ECC --  " ;
    if( m_trtHitsEC_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_trtHitsEC_cut << endreq;

    m_log << MSG::INFO << " Cutting on total Trt hits -----  ";
    if( m_totalTrtHits_cut < 0.) m_log << MSG::INFO << "Off" << endreq;
    else m_log << MSG::INFO << m_totalTrtHits_cut << endreq;

    m_log << MSG::INFO << "===============================================" << endreq;
  }

  return HLT::OK;
}

//---------------------------------------------------------------------------------------------------------------------------------------------
HLT::ErrorCode TrigCountTrtHitsHypo::hltBeginRun() {
  // This initialisation has been moved into the event loop.
  // @see TrigCountTrtHitsHypo::checkDetectorMask
  if (msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << " TrigCountTrtHitsHypo will be initialized in hltExecute" << endreq; 
  return HLT::OK;
}
//-----------------------------------------------------------------------------

HLT::ErrorCode TrigCountTrtHitsHypo::checkDetectorMask() {
  m_hltExecuteInitialisationRun = true;
  m_log << MSG::DEBUG << "[TrigCountTrtHitsHypo::checkDetectorMask]  beginning run with this " << name() << endreq;

  const xAOD::EventInfo* evinfo = 0;
  if (store()->retrieve(evinfo).isFailure()) {
    if (msgLvl() <= MSG::ERROR) m_log << MSG::ERROR << "Cannot retrieve xAOD::EventInfo from SG for detmasks" << endreq;
    return HLT::SG_ERROR;
  }
  else {
    if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "xAOD::EventInfo Run Information [Run,Evt,Lumi,Time,BunchCross,DetMask] = [" 
      << evinfo->runNumber()
      << "," << evinfo->eventNumber()
      << "," << evinfo->lumiBlock()
      << "," << evinfo->timeStamp()
      << ":" << evinfo->timeStampNSOffset()
      << "," << evinfo->bcid()
      << ",0x" << std::hex << evinfo->detectorMask() << std::dec
      << "]" << endreq;

    uint64_t mask = evinfo->detectorMask();
    eformat::helper::DetectorMask decoder(mask);

    if (mask == 0) {
      if (msgLvl() <= MSG::INFO) m_log << MSG::INFO << "Detector Mask == 0. Assuming MC file and setting all of ID to ON." << endreq; 
      m_trt_barrel_a_side = true;
      m_trt_barrel_c_side = true;
      m_trt_endcap_a_side = true;
      m_trt_endcap_c_side = true;
    } else {
      m_trt_barrel_a_side = decoder.is_set(eformat::TRT_BARREL_A_SIDE);
      m_trt_barrel_c_side = decoder.is_set(eformat::TRT_BARREL_C_SIDE);
      m_trt_endcap_a_side = decoder.is_set(eformat::TRT_ENDCAP_A_SIDE);
      m_trt_endcap_c_side = decoder.is_set(eformat::TRT_ENDCAP_C_SIDE);
    }

    if( msgLvl() <= MSG::INFO ){
      m_log << MSG::INFO << "trt_barrel_a_side is " << (m_trt_barrel_a_side==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      m_log << MSG::INFO << "trt_barrel_c_side is " << (m_trt_barrel_c_side==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      m_log << MSG::INFO << "trt_endcap_a_side is " << (m_trt_endcap_a_side==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq;
      m_log << MSG::INFO << "trt_endcap_c_side is " << (m_trt_endcap_c_side==true? "present" : "OFF! Warning: Check if this info is used in trigger decision!") << endreq; 
    }
  }
  return HLT::OK;
}

HLT::ErrorCode TrigCountTrtHitsHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  // Do initialisation at start of first event
  if (m_hltExecuteInitialisationRun == false) {
    HLT::ErrorCode ec = checkDetectorMask();
    if (ec != HLT::OK) return ec;
  }

  if(msgLvl() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "Executing this TrigCountTrtHitsHypo " << name() << endreq;
  }

  if(m_acceptAll){
    pass=true;
    if(msgLvl() <= MSG::DEBUG) m_log << MSG::DEBUG << "Accepting all events in " << name() << endreq;
    return HLT::OK;
  }

  if( !(m_trt_barrel_a_side && m_trt_barrel_c_side && m_trt_endcap_a_side && m_trt_endcap_c_side) ){
    m_log << MSG::DEBUG << "Trt detector is not present. DEBUG will be a WARNING and all events will be rejected in future." << endreq;
    //    pass=false;
    //    return HLT::OK;

  }

  pass=false;
  m_nTrtHits = 0.;
  m_nTrtHits_B = 0.;
  m_nTrtHits_A = 0.;
  m_nTrtHits_C = 0.;

  const TrigTrtHitCounts* trthits = 0;  
  HLT::ErrorCode hltStatus = getFeature(outputTE, trthits, "TrtHitCount");
  
  if(hltStatus != HLT::OK || !trthits){
    if(msgLvl() <= MSG::WARNING){
      m_log << MSG::WARNING << "Failed to retrieve features from TE." << endreq;
    }
    return HLT::OK;
  }
  
  m_nTrtHits_C += trthits->endcapC().sumEntries(m_timeOverThresholdCut, TrigHistoCutType::ABOVE_X);
  m_nTrtHits_B += trthits->barrel().sumEntries(m_timeOverThresholdCut, TrigHistoCutType::ABOVE_X);
  m_nTrtHits_A += trthits->endcapA().sumEntries(m_timeOverThresholdCut, TrigHistoCutType::ABOVE_X);
  m_nTrtHits = m_nTrtHits_A + m_nTrtHits_B + m_nTrtHits_C;
    
  // require a minimum of hits in one endcap
  if( m_trtHitsEC_cut >= 0. ){
    if( m_nTrtHits_A > m_nTrtHits_C ){
      m_maxTrtHitsEC = m_nTrtHits_A;
      m_log << MSG::DEBUG << "More hits in trt endcap A" << endreq;
    }
    else{
      m_maxTrtHitsEC = m_nTrtHits_C;
      m_log << MSG::DEBUG << "More hits in trt endcap C" << endreq;
    }
    
    // trigger decision
    if( m_maxTrtHitsEC >= m_trtHitsEC_cut ){
      pass=true;
      if( msgLvl() <= MSG::DEBUG ){      
	m_log << MSG::DEBUG <<  "REGTEST: event passed with " << m_maxTrtHitsEC << endreq;     
      }
    }
    else{ 
      if( msgLvl() <= MSG::DEBUG )
	m_log << MSG::DEBUG <<  "REGTEST: event failed with " << m_maxTrtHitsEC << endreq;
    }
  }
  
  // require a total number of hits
  else if( m_totalTrtHits_cut >= 0. ){
    
    // trigger decision
    if( m_nTrtHits >= m_totalTrtHits_cut ) pass=true;
    m_log << MSG::DEBUG <<  "REGTEST: event passed with " << m_nTrtHits << endreq;          
  }
  else 
    m_log << MSG::DEBUG <<  "REGTEST: event failed with " << m_nTrtHits << endreq;
  
  return HLT::OK;  
}

//-----------------------------------------------------------------------------
  
HLT::ErrorCode TrigCountTrtHitsHypo::hltFinalize() {
  if(msgLvl() <= MSG::INFO) {
    m_log << MSG::INFO << " finalizing this TrigCountTrtHitsHypo : " << name() << endreq; 
  }
  return HLT::OK;  
}

//-----------------------------------------------------------------------------
