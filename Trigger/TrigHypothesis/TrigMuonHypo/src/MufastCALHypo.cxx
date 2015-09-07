/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigMuonHypo/MufastCALHypo.h"


class ISvcLocator;

MufastCALHypo::MufastCALHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
	
	declareProperty("AcceptAll", m_acceptAll=false);
	
	declareMonitoredVariable("Pt",  m_fex_pt);
	declareMonitoredVariable("Eta", m_fex_eta);
	declareMonitoredVariable("Phi", m_fex_phi);
	
        m_storeGate = 0;
}

MufastCALHypo::~MufastCALHypo(){
}

HLT::ErrorCode MufastCALHypo::hltInitialize(){

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


HLT::ErrorCode MufastCALHypo::hltFinalize(){
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode MufastCALHypo::hltExecute(const HLT::TriggerElement* outputTE, 
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
  
   // Get vector of pointers to L2StandAloneMuon linked to the outputTE 
   const xAOD::L2StandAloneMuonContainer* vectorOfMuons(0);
   HLT::ErrorCode status = getFeature(outputTE, vectorOfMuons);
   if (status!=HLT::OK) {
      msg() << MSG::DEBUG << "no L2StandAloneMuon found" << endreq;
      return status;
   }

   // Check that there is only one L2StandAloneMuon
   if (vectorOfMuons->size() != 1){
      msg() << MSG::ERROR << "Size of vector is " << vectorOfMuons->size() << endreq;
      return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
   }

   // Get first (and only) RoI:
   const xAOD::L2StandAloneMuon* pMuon = vectorOfMuons->front();
   if(!pMuon){
      msg() << MSG::ERROR << "Retrieval of L2StandAloneMuon from vector failed" << endreq;
      return HLT::NAV_ERROR;
   }


  // fill Monitoring histos
  m_fex_pt  = (pMuon->pt())? pMuon->pt()  : -9999;
  m_fex_eta = (pMuon->etaMS())? pMuon->etaMS() : -9999;
  m_fex_phi = (pMuon->etaMS())? pMuon->phiMS() : -9999; 

  /* warning in coverity report: result is always false
  if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " Result of L2 Muon Calibration Hypothesis is " 
            << (result?"true":"false") << endreq;
  }
  */
  
  //store the result
  pass = result;
  
  return HLT::OK;
}
