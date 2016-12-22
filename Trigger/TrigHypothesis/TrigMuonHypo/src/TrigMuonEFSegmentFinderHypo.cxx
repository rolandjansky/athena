/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonHypo/TrigMuonEFSegmentFinderHypo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigMuonEFSegmentFinderHypo::TrigMuonEFSegmentFinderHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){
  declareProperty("AcceptAll", m_acceptAll=false);
  declareProperty("Nseg" , m_nseg=0);  

  m_storeGate=0;
}

TrigMuonEFSegmentFinderHypo::~TrigMuonEFSegmentFinderHypo(){
}

HLT::ErrorCode TrigMuonEFSegmentFinderHypo::hltInitialize(){

  if(m_acceptAll) {
    msg() << MSG::DEBUG
          << "Accepting all the events with not cut!"
          << endmsg;
  }    
 
  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endmsg;

  return HLT::OK;
}

HLT::ErrorCode TrigMuonEFSegmentFinderHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}

HLT::ErrorCode TrigMuonEFSegmentFinderHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endmsg;


  if(m_acceptAll) {
    pass = true;
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG 
            << "Accept property is set: taking all the events"
            << endmsg;
    }
    return HLT::OK;
  }
  
  bool result = false;
  bool debug = msgLvl() <= MSG::DEBUG;

  // Some debug output:
  if(debug) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endmsg;


  //  will do hypo cuts here
  
  // Get muonContainer linked to the outputTE 

  const xAOD::MuonContainer* muonContainer=0;
  if(getFeature(outputTE, muonContainer)!=HLT::OK) {
    if (debug) msg() << MSG::DEBUG << "no xAOD::Muon Feature found" << endmsg;
    return HLT::MISSING_FEATURE;
  } else {
    if (!muonContainer) { 
      if (debug) msg() << MSG::DEBUG << "null xAOD::MuonContainer Feature found" << endmsg; 
      return HLT::MISSING_FEATURE; 
    } 
    if (debug) msg() << MSG::DEBUG << "xAOD::Muon container found with size " << muonContainer->size() << endmsg;
  } 

 
  // loop on the muons within the RoI
  for (unsigned int j=0; j<muonContainer->size(); j++ ) {
    const xAOD::Muon* muon = muonContainer->at(j);
    if (!muon) {
      if (debug) msg() << MSG::DEBUG << "No xAOD::Muon found." << endmsg;
      continue;
    } else {
      if (muon->nMuonSegments() >= m_nseg){
	result = true;
      } else {
	result=false;
      }
      if(debug) msg() << MSG::DEBUG << "REGTEST the total number of reconstruced segments is " << muon->nMuonSegments()    
		      << " and threshold cut is " << m_nseg  
		      << " so hypothesis is " << (result?"true":"false") << endmsg;
    }
  }

  pass= result;
  return HLT::OK;
}  

