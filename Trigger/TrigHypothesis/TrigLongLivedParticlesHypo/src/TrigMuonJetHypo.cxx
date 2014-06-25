/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   FILE NAME:   TrigMuonJetHypo.cxx
 **   PACKAGE:     Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
 **
 **   DESCRIPTION: Hyphotesis algorithm developped for the 
 **                muon-jet selection 
 **
 **************************************************************************/ 


#include "TrigLongLivedParticlesHypo/TrigMuonJetHypo.h"
//** trigger stuff **//
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigInterfaces/FexAlgo.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigSteeringEvent/PhiHelper.h"
//** standard stuff **//
#include "TMath.h"
#include <map>
#include <cmath>
//** trigger EDM **//
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigInterfaces/ComboAlgo.h"
//*********----------------------------------------------------------------------------------*********//


class ISvcLocator;


TrigMuonJetHypo::TrigMuonJetHypo(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator)
{
  mJetEne = 0;
  mNumTrk = 0;
  mdeltaR = 0;


  //monitored variables
  declareMonitoredVariable("JetEne", mJetEne);
  declareMonitoredVariable("NumTrk", mNumTrk);
  declareMonitoredVariable("deltaR", mdeltaR);

  //** Read cuts **//
  declareProperty("AcceptAll",            m_acceptAll      = false);
  declareProperty("deltaRMatching",       m_deltaRMatching = true);
  declareProperty("deltaRCut",            m_deltaRCut      = 0.4);
  declareProperty("UseL2Muons",           m_useL2Muons     = true);
  declareProperty("Instance",             m_instance);

  //ID
  declareProperty("UseID",               m_UseID = true, "to use or not the ID informations");
  declareProperty("IDalgo",              m_ID_algo_to_use = "STRATEGY_B", "");
  declareProperty("Deta",                m_Deta = 0.2, "cut value for EtaJet-EtaTrack");
  declareProperty("Dphy",                m_Dphi = 0.2, "cut value for PhiJet-PhiTrack");
  declareProperty("JetEta",              m_JetEta = 2.5, "cut value for Eta of the Jet");
  declareProperty("PtMinID",             m_PtMinID = 800.0, "minimum Pt (MeV) of the ID tracks");
  declareProperty("nTracks",             m_nTracks = 0, "number of ID tracks in the Dphi*Deta");  
}


//*********----------------------------------------------------------------------------------*********//


TrigMuonJetHypo::~TrigMuonJetHypo() {}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigMuonJetHypo::hltInitialize() {


  if (m_instance == "L2"){
    if (msgLvl() <= MSG::INFO) 
      msg() << MSG::INFO << " ***** Initializing TrigMuonJetHypo on L2 ***** " << endreq;
    
    
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " Initialization completed successfully:"   << endreq;
      msg() << MSG::DEBUG << " - Instance = "       << m_instance        << endreq;
      msg() << MSG::DEBUG << " - AcceptAll is "
            << (m_acceptAll == true ? "true" : "false") << endreq;
    }

    //Set ID Algo to use
    std::string algoId = m_ID_algo_to_use;

    if      (algoId=="IDSCAN")   m_algoId = TrigInDetTrack::IDSCANID;
    else if (algoId=="SITRACK")  m_algoId = TrigInDetTrack::SITRACKID;
    else if (algoId=="STRATEGY_A") m_algoId = TrigInDetTrack::STRATEGY_A_ID;
    else if (algoId=="STRATEGY_B") m_algoId = TrigInDetTrack::STRATEGY_B_ID;
    else if (algoId=="STRATEGY_C") m_algoId = TrigInDetTrack::STRATEGY_C_ID;
    else if (algoId=="TRTXK")      m_algoId = TrigInDetTrack::TRTXKID;
    else if (algoId=="TRTSEG")     m_algoId = TrigInDetTrack::TRTLUTID;
    else                         m_algoId = TrigInDetTrack::IDSCANID;
  }
  
  if (m_instance == "EF"){
    if (msgLvl() <= MSG::INFO) 
      msg() << MSG::INFO << " ***** Initializing TrigMuonJetHypo on EF ***** " << endreq;
    
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << " Initialization completed successfully:"   << endreq;
      msg() << MSG::DEBUG << " - Instance = "       << m_instance        << endreq;
    }
  }

  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigMuonJetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  pass = false;

  if (m_instance == "L2"){
    
    if (msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << " ***** Executing TrigMuonJetHypo on L2   ***** " << endreq;
    }
    
    if (m_acceptAll) {
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " AcceptAll property is set: taking all events. "      << endreq;
      pass = true;
      return HLT::OK;
    } 
    else {
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " AcceptAll property is not set: applying selection. " << endreq;
    }
  
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
 
    //** deltaR Matching mode **//
    if (m_deltaRMatching) { 
      
      double muonEta=0,jetEta=0,muonPhi=0,jetPhi=0;
      
      //** This are 2 TEs which we eventually will combine **//
      const HLT::TriggerElement* TE1 = vectorOfInputTE[1];	
      const HLT::TriggerElement* TE2 = vectorOfInputTE[0];

      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " deltaR estimation using " << (m_useL2Muons ? " L2 " : " L1 ") << " muons and L2 jets" << endreq;
      
      // ACCESS L1/L2 MUON
      const TrigRoiDescriptor* L1Muon = 0;
      std::vector<const MuonFeature*> vectorOfMuons;
      HLT::ErrorCode statusMuon;

      if (m_useL2Muons) {
	statusMuon = getFeatures(TE1, vectorOfMuons);
	if(statusMuon!=HLT::OK) {
	  msg() << MSG::WARNING << " Failed to get the L2 Muons" << endreq;
	  return statusMuon;
	}

	// Check that there is only one MUON
	if (vectorOfMuons.size() != 1){
	  msg() << MSG::ERROR << "The number of Muons attached to the TE is not 1" << endreq;
	  return HLT::OK;
	}
      } else {
	statusMuon = getFeature(TE1, L1Muon);
	if(statusMuon!=HLT::OK) {
	  msg() << MSG::WARNING << " Failed to get the L1 Muons" << endreq;
	  return statusMuon;
	}
      }

      // ACCESS L2 JET
      std::vector<const TrigT2Jet*> vectorOfJets;
      HLT::ErrorCode statusJet = getFeatures(TE2, vectorOfJets);
      if(statusJet!=HLT::OK) {
	msg() << MSG::WARNING << " Failed to get the L2 Jets" << endreq;
	return statusJet;
      }

      // Check that there is only one JET
      if (vectorOfJets.size() != 1){
	msg() << MSG::ERROR << "The number of Jets attached to the TE is not 1" << endreq;
	return HLT::OK;
      }
      
      // GET MUON
      if (m_useL2Muons) {
	const MuonFeature* pMuon = vectorOfMuons.front();

	if(!pMuon){
	  msg() << MSG::ERROR << "Retrieval of muon from vector failed"  << endreq;
	  return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
	}

	muonEta = pMuon->eta();
	muonPhi = pMuon->phi();

      } else {

	muonEta = L1Muon->eta();
	muonPhi = L1Muon->phi();
      }

      // GET JET
      const TrigT2Jet* pJet = vectorOfJets.front();
      if(!pJet){
	msg() << MSG::ERROR << "Retrieval of jet from vector failed"  << endreq;
	return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
      }

      jetEta = pJet->eta();
      jetPhi = pJet->phi();
      
      //** Selection is done here **//
      //**                        **//
      //**  Debug dump            **// 
      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << " New combination: "                 << endreq; 
	msg() << MSG::DEBUG << " - Muon features: "                 << endreq; 
	msg() << MSG::DEBUG << "  -- etaL2 = "       << muonEta     << endreq;
	msg() << MSG::DEBUG << "  -- phiL2 = "       << muonPhi     << endreq;
	msg() << MSG::DEBUG << " - Jet features: "                  << endreq; 
	msg() << MSG::DEBUG << "  -- etaL2 = "       << pJet->eta() << endreq;
	msg() << MSG::DEBUG << "  -- phiL2 = "       << pJet->phi() << endreq;
      }
 
      //monitored variable
      double etjet = pJet->et();
      mJetEne = etjet/1000;
 
      double delta_eta = muonEta - jetEta;
      double delta_phi = HLT::wrapPhi(muonPhi - jetPhi);
            
      double dR = sqrt( (delta_eta*delta_eta) + (delta_phi*delta_phi) );
      //monitored variable
      mdeltaR = dR;

      bool passCutDR = false;
      if ( dR < m_deltaRCut) {
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG  << " COMBINATION PASSED: deltaR matching found! " << endreq;
	  msg() << MSG::DEBUG << "  - deltaR = " << dR                           << endreq;
	}
        passCutDR = true;
      } else {
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG  << " COMBINATION FAILED: no deltaR matching found!" << endreq; 
	  msg() << MSG::DEBUG << "  - deltaR = " << dR                             << endreq;
	}
      }
      
      bool passCutEta = false;
      if ( fabs(pJet->eta()) < m_JetEta ) passCutEta = true;
      if( (passCutEta && passCutDR) || m_acceptAll ) {
	
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Accessing the ID track collection " << endreq;
	//ACCESSING THE ID TRACK COLLECTION FOR THE TRACKLESS JET REQUEST
	int ntrk = 0;
	if(m_UseID) {
	  
	  HLT::ErrorCode status;
	  std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
	  status = getFeatures(TE2,vectorOfTrackCollections, "");
	  
	  if( status!=HLT::OK ) {
	    msg() << MSG::DEBUG << " Failed to get InDetTrackCollections " << endreq;
	  }else{ 
	    msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size() 
		  << " InDetTrackCollections " << endreq;  
	    
	    std::vector<const TrigInDetTrackCollection*>::iterator 
	      theTrackColl = vectorOfTrackCollections.begin(),
	      endTrackColl = vectorOfTrackCollections.end();
	    
	    for( ; theTrackColl != endTrackColl;  theTrackColl++){
	      
	      TrigInDetTrackCollection::const_iterator 
		track     = (*theTrackColl)->begin(),
		lasttrack = (*theTrackColl)->end();
	      
	      for(; track !=lasttrack; track++ ){

		if( (*track)->algorithmId() != m_algoId ) continue;
		if (!((*track)->param())) continue;
		
		double pt_idtr = (*track)->param()->pT();
		if (fabs(pt_idtr) <= m_PtMinID) continue;
		
		double phi_id  = (*track)->param()->phi0();
		double eta_id  = (*track)->param()->eta();
		
		if(msgLvl() <= MSG::DEBUG)
		  msg() << MSG::DEBUG << "trying " << m_ID_algo_to_use << " track with " << "pt=" << pt_idtr << ", eta=" << eta_id << ", phi=" << phi_id  << endreq;
		
		double deta = fabs( eta_id - jetEta );
		double dphi = fabs( HLT::wrapPhi(phi_id - jetPhi) );
		
		if(deta <= m_Deta && dphi <= m_Dphi){ 
		  ntrk++;
		}
	      }
	    }
	  }
	}
        //monitored variable
        mNumTrk = ntrk;

	if(ntrk <= m_nTracks){
	  pass = true;
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Number of tracks in ID is " << ntrk << ".... Event Accepted!" << endreq;
	}else{
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Number of tracks in ID is " << ntrk << ".... Event rejected !" << endreq;
	} 
      } else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Event rejected !" << endreq;
      }
    }
  }
  

  if (m_instance == "EF"){ 
    
    if (msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << " ***** Executing TrigMuonJetHypo on EF   ***** " << endreq;
      msg() << MSG::DEBUG << " ----- Dummy algorithm: accepts all events ----- " << endreq;
    }

    pass = true;

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Passing Trigger " << endreq;
    
  }
  
  
  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//


HLT::ErrorCode TrigMuonJetHypo::hltFinalize()
{
  msg() << MSG::INFO << " ***** Finalizing TrigMuonJetHypo ***** " << endreq;
  
  return HLT::OK;
}


//*********----------------------------------------------------------------------------------*********//

