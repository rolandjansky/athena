/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigMuonHypo/MufastStauHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MufastStauHypo::MufastStauHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){

   std::vector<float> def_bins;
   def_bins.push_back(0);
   def_bins.push_back(2.5);
   std::vector<float> def_thrs;
   def_thrs.push_back(40*CLHEP::GeV);

   declareProperty("PtBins",       m_ptBins=def_bins);
   declareProperty("PtThresholds", m_ptThresholds=def_thrs);

   declareProperty("BetaMax", m_betaMax = 0.97);
   declareProperty("MMin", m_mMin = 80.*CLHEP::GeV);
   declareProperty("AcceptAll", m_acceptAll=false);
   declareProperty("EtaCut", m_etaCut=false);

   declareMonitoredVariable("Pt",  m_fex_pt);
   declareMonitoredVariable("Beta",m_fex_beta);
   declareMonitoredVariable("Mass",m_fex_mass);
   declareMonitoredVariable("Eta", m_fex_eta);
   declareMonitoredVariable("Phi", m_fex_phi);

   m_storeGate = 0;
}

MufastStauHypo::~MufastStauHypo(){
}

HLT::ErrorCode MufastStauHypo::hltInitialize(){

  if(m_acceptAll) {
      msg() << MSG::INFO
            << "Accepting all the events with not cut!"
	    << endreq;
  } else {
     m_bins = m_ptBins.size() - 1;
     if (m_bins != m_ptThresholds.size()) {
        msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endreq;
        return HLT::BAD_JOB_SETUP;
     } 
  }

  for (std::vector<float>::size_type i=0; i<m_bins;++i) {
      char buf1[256];
      char buf2[256];
      sprintf(buf1,"%f",m_ptBins[i]);
      sprintf(buf2,"%f",m_ptBins[i+1]);
      msg() << MSG::INFO << "EtaBin " << buf1 << " - " <<  buf2
            << ": with Pt Threshold of " << (m_ptThresholds[i])/CLHEP::GeV
            << " GeV" << endreq;
  }
      msg() << MSG::INFO
            << "Set BetaMax to " << m_betaMax << " GeV"
            << "Set MMin to " << m_mMin/CLHEP::GeV << " GeV"
            << endreq;

     
 
  msg() << MSG::INFO 
        << "Initialization completed successfully" 
        << endreq;
  
  return HLT::OK;
}


HLT::ErrorCode MufastStauHypo::hltFinalize(){
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode MufastStauHypo::hltExecute(const HLT::TriggerElement* outputTE, 
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
     return HLT::NAV_ERROR;
  }

  // Get first (and only) RoI:
  const xAOD::L2StandAloneMuon* pMuon = vectorOfMuons->front();
  if(!pMuon){
    msg() << MSG::ERROR << "Retrieval of L2StandAloneMuon from vector failed" << endreq;
    return HLT::NAV_ERROR;
  }

  m_fex_pt  = (pMuon->pt())? pMuon->pt()  : -9999.;
  m_fex_eta = (pMuon->etaMS())? pMuon->etaMS() : -9999.;
  m_fex_phi = (pMuon->etaMS())? pMuon->phiMS() : -9999.;


  //Get the Pt cut for that eta bin
  float threshold = 0;
  float absEta = fabs(m_fex_eta);
  for (std::vector<float>::size_type i=0; i<m_bins; ++i)
     if ( absEta > m_ptBins[i] && absEta < m_ptBins[i+1] ) threshold = m_ptThresholds[i];



  // Check pt threshold for hypothesis, 
  // convert units since Muonfeature is in GeV
 double mCand = 0;
 double BetaCand = pMuon->beta();
 if (fabsf(pMuon->etaMS())>1.5 && fabsf(pMuon->etaMS())<1.8 && m_etaCut==true) 
    result = false; 
 else if (fabsf(pMuon->etaMS())<=1.05)
 {
    
	//if ( fabsf(pMuon->pt()) > (m_ptThreshold/CLHEP::GeV)&& BetaCand < m_betaMax && BetaCand>0)
        if ( std::abs(pMuon->pt()) > (threshold/CLHEP::GeV)&& BetaCand < m_betaMax && BetaCand>0)
	{
		double theta = 2.*atan(exp(-pMuon->etaMS())); //should be turned into codes
		double pCand = std::abs(pMuon->pt())/sin(theta)*CLHEP::GeV;
		mCand = pCand * sqrt(1.-BetaCand*BetaCand)/BetaCand; // should be turned into code
		if (mCand > m_mMin) result = true;
	}
	  
        //if (msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << " REGTEST muon mass is " << mCand/CLHEP::GeV << " GeV" 
                  << " and threshold cut is " << m_mMin/CLHEP::GeV << " GeV" 
                  << " so hypothesis is " << (result?"true":"false") << endreq;
        //}
  
 }
 else 
 {
    //if ( fabsf(pMuon->pt()) > (m_ptThreshold/CLHEP::GeV))
    if ( std::abs(pMuon->pt()) > (threshold/CLHEP::GeV))
    {
        result = true;   
    }
    msg() << MSG::DEBUG << " REGTEST muon pt  is " << (fabs(pMuon->pt())) << " GeV" 
          << " and threshold cut is " << threshold/CLHEP::GeV << " GeV" 
          << " so hypothesis is " << (result?"true":"false") << endreq;
 }  
        //store the result
        pass = result;

 m_fex_beta = BetaCand;
 m_fex_mass = mCand;

  return HLT::OK;
}

