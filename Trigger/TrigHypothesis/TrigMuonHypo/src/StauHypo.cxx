/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonHypo/StauHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

StauHypo::StauHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){
	declareProperty("PtThreshold", m_ptThreshold = 30.*CLHEP::GeV);
	declareProperty("BetaMax", m_betaMax = 0.97);
	declareProperty("MMin", m_mMin = 40.*CLHEP::GeV);
	declareProperty("AcceptAll", m_acceptAll=false);
}

StauHypo::~StauHypo(){
}

HLT::ErrorCode StauHypo::hltInitialize(){

  if(m_acceptAll) {
      msg() << MSG::INFO
            << "Accepting all the events with not cut!"
	    << endreq;
  } else {
      msg() << MSG::INFO
            << "Set PtThreshold to " << m_ptThreshold/CLHEP::GeV << " GeV"
            << "Set BetaMax to " << m_betaMax << " GeV"
            << "Set MMin to " << m_mMin/CLHEP::GeV << " GeV"
            << endreq;
  }
 
  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endreq;
  
  return HLT::OK;
}


HLT::ErrorCode StauHypo::hltFinalize(){
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode StauHypo::hltExecute(const HLT::TriggerElement* outputTE, 
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

  // Check pt threshold for hypothesis, 
  // convert units since Muonfeature is in GeV
	double mCand = 0;
	double BetaCand = pMuon->beta();
	if ( fabsf(pMuon->pt()) > (m_ptThreshold/CLHEP::GeV)&& BetaCand < m_betaMax)
	{
		double theta = 2.*atan(exp(-pMuon->eta())); //should be turned into codes
		double pCand = fabsf(pMuon->pt())/sin(theta)*CLHEP::GeV;
		mCand = pCand * sqrt(1.-BetaCand*BetaCand)/BetaCand; // should be turned into code
		if (mCand > m_mMin) result = true;
	}
	  
  if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " REGTEST muon mass is " << mCand << " GeV" 
            << " and threshold cut is " << m_mMin/CLHEP::GeV << " GeV" 
            << " so hypothesis is " << (result?"true":"false") << endreq;
  }
  
  //store the result
  pass = result;
  
  return HLT::OK;
}

