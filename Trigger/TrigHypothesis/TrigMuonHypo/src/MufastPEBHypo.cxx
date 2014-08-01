/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonHypo/MufastPEBHypo.h"


class ISvcLocator;

MufastPEBHypo::MufastPEBHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
	
	declareProperty("AcceptAll", m_acceptAll=false);
	
	declareMonitoredVariable("Pt",  m_fex_pt);
	declareMonitoredVariable("Eta", m_fex_eta);
	declareMonitoredVariable("Phi", m_fex_phi);
	
}

MufastPEBHypo::~MufastPEBHypo(){
}

HLT::ErrorCode MufastPEBHypo::hltInitialize(){

  msg() << MSG::INFO << "Initializing " << name() << " - package version " 
	<< PACKAGE_VERSION << endreq;

  if(m_acceptAll) {
      msg() << MSG::INFO
            << "Accepting all the events with not cut!"
	    << endreq;
  } 
 
  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endreq;
  
  return HLT::OK;
}


HLT::ErrorCode MufastPEBHypo::hltFinalize(){
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode MufastPEBHypo::hltExecute(const HLT::TriggerElement* outputTE, 
                                      bool& pass){

  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;
  
  if(m_acceptAll) {
      pass = true;
      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG 
	        << "Accept property is set: taking all the events"
		<< endreq;
      }
      return HLT::OK;
  }
  
  if(msgLvl() <= MSG::DEBUG) {
       msg() << MSG::DEBUG << "Accept property not set: applying selection!"
             << endreq;
  }
  
  
  bool result = false;
  
  
  // Some debug output:
  if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;
  }
  

  // Get vector of pointers to all MuonFeatures linked to the outputTE 
  //   by label "uses":
  std::vector<const MuonFeature*> vectorOfMuons; 
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfMuons);
  if(status!=HLT::OK) {
      msg() << MSG::DEBUG << "no MuonFeatures found" << endreq;
      return status;
  }
  // Check that there is only one MuonFeature
  if (vectorOfMuons.size() != 1){
    msg() << MSG::ERROR << "Size of vector is " << vectorOfMuons.size()
          << endreq;
    return HLT::NAV_ERROR;
  }

  // Get first (and only) RoI:
  const MuonFeature* pMuon = vectorOfMuons.front();
  if(!pMuon){
    msg() << MSG::ERROR << "Retrieval of MuonFeature from vector failed"
          << endreq;
    return HLT::NAV_ERROR;
  }

  // fill Monitoring histos
  m_fex_pt  = (pMuon->pt())? pMuon->pt()  : -9999;
  m_fex_eta = (pMuon->eta())? pMuon->eta() : -9999;
  m_fex_phi = (pMuon->eta())? pMuon->phi() : -9999;

  if(pMuon->sp1_z() != 0 || pMuon->sp2_z() != 0 || pMuon->sp3_z()) {
      result = true;
  } 

  if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " Super Point 1, z = " << pMuon->sp1_z() <<endreq; 
      msg() << MSG::DEBUG << " Super Point 2, z = " << pMuon->sp2_z() <<endreq; 
      msg() << MSG::DEBUG << " Super Point 3, z = " << pMuon->sp3_z() <<endreq; 
      msg() << MSG::DEBUG << " Result of Partial Event Build Hypothesis is " 
            << (result?"true":"false") << endreq;
  }
  
  //store the result
  pass = result;
  
  return HLT::OK;
}
