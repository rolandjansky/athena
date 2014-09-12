/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C++  
// :-)
/**************************************************************************
 **
 **   FILE NAME:   TrigLeptonJetHypo.cxx
 **   PACKAGE:     Trigger/TrigHypothesis/TrigBjetHypo
 **
 **   DESCRIPTION: Hyphotesis algorithm developped for the 
 **                muon-jet selection 
 **            
 **
 **   AUTHOR: Cristiano Alpigiani 
 **   EMAIL:  Cristiano.Alpigiani@ge.infn.it
 **
 **   CREATED:  April, 2008
 **   MODIFIED: September, 2009 (Andrea Coccaro)
 **              
 **************************************************************************/ 

#include "TrigBjetHypo/TrigLeptonJetHypo.h"

#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"



//*********----------------------------------------------------------------------------------*********//


TrigLeptonJetHypo::TrigLeptonJetHypo(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator)
{
  declareProperty("AcceptAll",      m_acceptAll      = false);
  declareProperty("deltaRCut",      m_deltaRCut      = 0.4);
  declareProperty("Instance",       m_instance);
  declareProperty("LVL2Refining",   m_LVL2refining   = true);  

  declareMonitoredVariable("CutCounter", m_cutCounter);
}


//*********----------------------------------------------------------------------------------*********//


TrigLeptonJetHypo::~TrigLeptonJetHypo() {}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigLeptonJetHypo::hltInitialize() {

  //** Retrieve TrigTimerSvc service **//
  if (timerSvc())
    m_totTimer = addTimer("TrigLeptonJetHypo.TrigLeptonJetHypoTot");

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigLeptonJetHypo" << endreq;

  if (m_instance == "L2"){    
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "declareProperty review:" << endreq;
      msg() << MSG::DEBUG << " Instance = " << m_instance << endreq;
      msg() << MSG::DEBUG << " deltaRCut = " << m_deltaRCut << endreq;
      msg() << MSG::DEBUG << " LVL2Refining = " << (m_LVL2refining == true ? "true" : "false") << endreq; 
    }
  }  
  
  if (m_instance == "EF") {
    if (msgLvl() <= MSG::DEBUG) { 
      msg() << MSG::DEBUG << "declareProperty review:" << endreq;
      msg() << MSG::DEBUG << " Instance = " << m_instance << endreq;
    }
  }

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Initialization completed successfully" << endreq;

  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigLeptonJetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  //** Start of execution time TrigLeptonJetHypo algorithm **//
  if (timerSvc())
    m_totTimer->start();
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigLeptonJetHypo" << endreq;

  //* initialise monitoring variables *//
  m_cutCounter = -1;

  //* AcceptAll declare property setting *//
  if (m_acceptAll) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: AcceptAll property is set: taking all events" << endreq;
    m_cutCounter = 1;
    pass = true;
    return HLT::OK;
  } else 
    msg() << MSG::DEBUG << "REGTEST: AcceptAll property not set: applying selection" << endreq;

  bool result = false;
  double muonEta=0, jetEta=0, muonPhi=0, jetPhi=0;

  if (m_instance == "L2"){
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Executing TrigLeptonJetHypo on L2" << endreq;

    //* get input TE: use getVectorOfObjects() *//
    std::vector<HLT::TriggerElement*> vectorOfInputTE;
    vectorOfInputTE = HLT::Navigation::getDirectPredecessors(outputTE);
    
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Retrieved " << vectorOfInputTE.size() << " input trigger Elements" << endreq;

    if(vectorOfInputTE.size() != 2) {
      if(msgLvl() <= MSG::WARNING) {
	msg() << MSG::WARNING << "Retrieved the wrong number of input trigger elements: found "
	      << vectorOfInputTE.size() << " and expected 2" << endreq;
      }
      return HLT::NAV_ERROR;
    }

    //* to separate bad input TE and true behaviour *//
    m_cutCounter++;

    const HLT::TriggerElement* TE1 = vectorOfInputTE[0];	
    const HLT::TriggerElement* TE2 = vectorOfInputTE[1];

    const CombinedMuonFeature* CombinedMuon = 0; 
    const TrigRoiDescriptor*   MuonRoIDescriptor = 0;
    const TrigRoiDescriptor*   JetRoIDescriptor  = 0;

    if (m_LVL2refining){

      if(msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << " deltaR estimation with L2 refining using muComb algorithm" << endreq;        

      HLT::ErrorCode statusMuon = getFeature(TE1, CombinedMuon);
      HLT::ErrorCode statusJet  = getFeature(TE2, JetRoIDescriptor);

      if (statusMuon != HLT::OK || statusJet  != HLT::OK) {

	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Failed to retrieve feature using getFeature() method" << endreq;

	return HLT::NAV_ERROR;

      } else {

	if ( msgLvl() <= MSG::DEBUG ){
	  msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << endreq;
	  msg() << " - " << *CombinedMuon << endreq;
	  msg() << " - " << *JetRoIDescriptor << endreq;
	}

	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << " New combination: " << endreq; 
	  msg() << MSG::VERBOSE << " - Muon features: " << endreq; 
	  msg() << MSG::VERBOSE << "  eta = " << CombinedMuon->eta() << "\tphi = " << CombinedMuon->phi() << endreq;
	  msg() << MSG::VERBOSE << " - Jet features: " << endreq;
	  msg() << MSG::VERBOSE << "  eta = " << JetRoIDescriptor->eta() << "\tphi = " << JetRoIDescriptor->phi() << endreq;
	}

	muonEta = CombinedMuon->eta();
	jetEta  = JetRoIDescriptor ->eta();
	muonPhi = CombinedMuon->phi();
	jetPhi  = JetRoIDescriptor ->phi();
      }

    } else {

      if(msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << " deltaR estimation without L2 refining" << endreq; 	

      HLT::ErrorCode statusMuon = getFeature(TE1, MuonRoIDescriptor);
      HLT::ErrorCode statusJet  = getFeature(TE2, JetRoIDescriptor);

      if (statusMuon != HLT::OK || statusJet  != HLT::OK) {

	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Failed to retrieve feature using getFeature() method" << endreq;

	return HLT::NAV_ERROR;

      } else {
	
	if ( msgLvl() <= MSG::DEBUG ){
	  msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << endreq;
	  msg() << " - " << *MuonRoIDescriptor << endreq;
	  msg() << " - " << *JetRoIDescriptor << endreq;
	}

	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << " New combination: " << endreq; 
	  msg() << MSG::VERBOSE << " - Muon features: " << endreq; 
	  msg() << MSG::VERBOSE << "  eta = " << MuonRoIDescriptor->eta() << "\tphi = " << MuonRoIDescriptor->phi() << endreq;
	  msg() << MSG::VERBOSE << " - Jet features: " << endreq;
	  msg() << MSG::VERBOSE << "  eta = " << JetRoIDescriptor->eta() << "\tphi = " << JetRoIDescriptor->phi() << endreq;
	}

	muonEta = MuonRoIDescriptor->eta();
	jetEta  = JetRoIDescriptor ->eta();
	muonPhi = MuonRoIDescriptor->phi();
	jetPhi  = JetRoIDescriptor ->phi();

      }
    }

    //* DeltaR estimation *//     
    if (muonPhi < 0) muonPhi += 2*M_PI;
    if (jetPhi  < 0) jetPhi  += 2*M_PI;
      
    double deltaEta = muonEta - jetEta;
    double deltaPhi = muonPhi - jetPhi;
      
    if (deltaPhi >  M_PI) deltaPhi -= 2*M_PI;
    if (deltaPhi < -M_PI) deltaPhi += 2*M_PI;
      
    double dR = sqrt(pow(deltaEta,2) + pow(deltaPhi,2));
            
    if (dR < m_deltaRCut) {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG  << "Combination passed: deltaR = " << dR << endreq;
      result = true;
    } else {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG  << "Combination failed: deltaR = " << dR << endreq;
    }
  }

  if (m_instance == "EF"){ 
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " ***** Executing TrigLeptonJetHypo on EF   ***** " << endreq;

    //* to separate bad input TE and true behaviour *//
    m_cutCounter++;

    result = true;
  }

  if(result) m_cutCounter++;
  pass = result;
    
  //** End of execution time TrigLeptonJetHypo algorithm **//
  if (timerSvc())
    m_totTimer->stop();
  
  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigLeptonJetHypo::hltFinalize() {

  msg() << MSG::INFO << "Finalizing TrigLeptonJetHypo" << endreq;
  
  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//

