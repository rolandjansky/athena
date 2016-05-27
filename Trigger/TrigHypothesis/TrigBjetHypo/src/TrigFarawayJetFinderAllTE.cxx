/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigFarawayJetFinderAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Katharine Leney
// EMAIL:    Katharine.Leney AT cern.ch
// 
// ************************************************

#include "TrigBjetHypo/TrigFarawayJetFinderAllTE.h"

#include "xAODMuon/MuonContainer.h"

#include "JetEvent/Jet.h"

// #include "TrigParticle/TrigEFBjetContainer.h"
// #include "TrigParticle/TrigEFBjet.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigSteeringEvent/PhiHelper.h"

// ----------------------------------------------------------------------------------------------------------------- 


TrigFarawayJetFinderAllTE::TrigFarawayJetFinderAllTE(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty ("MinDeltaRCut", m_minDeltaRCut = 0.5);
  declareProperty ("JetInputKey",  m_jetInputKey  = "SplitJet");
  declareProperty ("JetOutputKey", m_jetOutputKey = "FarawayJet");
  declareProperty ("EtaHalfWidth", m_etaHalfWidth = 0.4); // Need to add a safety check to make sure that 
  declareProperty ("PhiHalfWidth", m_phiHalfWidth = 0.4); // this is the same value as use in JetSplitter
}


// ----------------------------------------------------------------------------------------------------------------- 


TrigFarawayJetFinderAllTE::~TrigFarawayJetFinderAllTE() {}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigFarawayJetFinderAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Initializing TrigFarawayJetFinderAllTE" << endreq;

  // declareProperty overview
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " MinDeltaRCut = " << m_minDeltaRCut << endreq; 
    msg() << MSG::DEBUG << " JetInputKey  = " << m_jetInputKey  << endreq; 
    msg() << MSG::DEBUG << " JetOutputKey = " << m_jetOutputKey << endreq; 
    msg() << MSG::DEBUG << " EtaHalfWidth = " << m_etaHalfWidth << endreq; 
    msg() << MSG::DEBUG << " PhiHalfWidth = " << m_phiHalfWidth << endreq; 
  }
  
  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


float TrigFarawayJetFinderAllTE::phiCorr(float phi) {

  if (phi < -M_PI) phi += 2*M_PI;
  if (phi >  M_PI) phi -= 2*M_PI;

  return phi;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigFarawayJetFinderAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigFarawayJetFinderAllTE" << endreq;

  beforeExecMonitors().ignore();

  msg() << MSG::DEBUG 
        << "Number of input TEs is " <<  inputTE.size() 
        << " TE0: " << inputTE[0].size() 
        << " TE1: " << inputTE[1].size() <<  endreq;  

  if (inputTE.size() < 2) {
    msg() << MSG::ERROR << "Number of input TEs is " <<  inputTE.size() << " and not 2. Configuration problem." << endreq;  
    return HLT::MISSING_FEATURE;
  }

  if(inputTE[0].size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No muon TE found" << endreq;
    return HLT::OK;
  }

  if(inputTE[1].size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No jet TE found" << endreq;
    return HLT::OK;
  }

  HLT::TEVec allTEs;
  
  if ((inputTE.size()>0) && (inputTE[0].size()>0)) 
    allTEs.push_back(inputTE[0][0]);
  
  if ((inputTE.size()>0) && (inputTE[1].size()>0))
    for (unsigned int i=0; i<inputTE[1].size(); i++)
      allTEs.push_back(inputTE[1][i]);

  // ==============================
  // Retrieve HLT jets
  // ==============================

  for(unsigned int iJet=0; iJet<inputTE[1].size(); iJet++) {

    const xAOD::JetContainer* outJets(0);
    
    HLT::ErrorCode statJets = getFeature(inputTE[1][iJet], outJets, m_jetInputKey);

    if(statJets!=HLT::OK) {
      msg() << MSG::WARNING << " Failed to get JetCollections " << endreq;
      return HLT::OK;
    }
    //
    if( outJets == 0 ){
      msg() << MSG::DEBUG << " Got no JetCollections associated to the TE! " << endreq;
      return HLT::MISSING_FEATURE;
    }
    std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());
       
    //check size of JetCollection
    if( theJets.size() == 0 ){
      msg()<< MSG::DEBUG << " Size of JetCollection is 0! " << endreq;
      return HLT::OK;
    }
    else if( theJets.size() > 1 ){
      msg()<< MSG::DEBUG << " Size of JetCollection is " << theJets.size() << ".  Expecting only one jet per TE!" << endreq;
      return HLT::OK;
    }

    std::vector<const  xAOD::Jet*>::const_iterator Jet = theJets.begin();

    //std::cout << "=====================> Jet" << iJet << std::endl;
    
    float minimumDeltaR = 9e9;
    
    float jetEt  = (*Jet)->p4().Et()/1000.0;
    float jetEta = (*Jet)->eta();
    float jetPhi = (*Jet)->phi();
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG 
	    << "Jet "   << iJet 
	    << "; et "  << jetEt
	    << "; eta " << jetEta 
	    << "; phi " << jetPhi << endreq;
    
    // ==============================
    // Retrieve HLT muons
    // ==============================
    
    for(unsigned int im=0; im<inputTE[0].size(); im++) {
      const xAOD::MuonContainer* theMuons=0;
      //    HLT::ErrorCode statusMuons = getFeature(inputTE[0].front(), theMuons);
      HLT::ErrorCode statusMuons = getFeature(inputTE[0][im], theMuons);
      
      if (statusMuons != HLT::OK) {
	if (msgLvl() <= MSG::WARNING)
	  msg() << MSG::WARNING << "Failed to retrieve muons" << endreq;
	return HLT::NAV_ERROR;
      } 
      else if (theMuons==0) {
	if (msgLvl() <= MSG::WARNING)
	  msg() << MSG::WARNING << "Missing muon feature." << endreq;
	return HLT::MISSING_FEATURE;
      } 
      else if (theMuons->size()==0) {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Muon vector empty." << endreq;
	return HLT::OK;
      } 
      else {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Retrieved " << theMuons->size() << " muon." << endreq;
      } 

      // ================================================
      // Do we ever have more than one muon per TE???
      // If not, we can get rid of this second loop...
      // ================================================

      // Loop over muons
      int iMuon=-1;	
      for(auto Muon : *theMuons) {
	iMuon++;
	
	const xAOD::Muon::MuonType muontype = Muon->muonType();
	if(muontype != xAOD::Muon::MuonType::Combined) continue;
	
	float muonPt = Muon->pt();
	float muonEta = Muon->eta();
	float muonPhi = Muon->phi();
	
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG 
		<< "Muon "  << iMuon 
		<< "; pt "  << muonPt
		<< "; eta " << muonEta 
		<< "; phi " << muonPhi << endreq;
	
	// Find the minimum dR between this jet and any muon
	
	float m_deltaEta = muonEta - jetEta;
	float m_deltaPhi = phiCorr(phiCorr(muonPhi) - phiCorr(jetPhi));
	
	double dR = sqrt(m_deltaEta*m_deltaEta + m_deltaPhi*m_deltaPhi);
	
	if (msgLvl() <= MSG::DEBUG) 
	  msg() << MSG::DEBUG << "deltaR = "<< dR <<  endreq; 
	
	if (dR < minimumDeltaR) minimumDeltaR = dR;
	
      } // Finish loop over muons
      
      if (minimumDeltaR > m_minDeltaRCut) {
	
	if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet " << iJet << " not close to any muons. Keeping for b-tagging." <<  endreq; 
	
	HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
	
	// Create an output TE seeded by an empty vector
	HLT::TriggerElement* outputTE = config()->getNavigation()->addNode( initialTE, output );
	outputTE->setActiveState(true);
	
	// For the jets with dR(jet,muon) > 0.5 
	// create RoI correspondinding to the jet
	
	double phiMinus = HLT::wrapPhi(jetPhi-m_phiHalfWidth); 
	double phiPlus  = HLT::wrapPhi(jetPhi+m_phiHalfWidth); 
	
	double etaMinus = jetEta-m_etaHalfWidth;  
	double etaPlus  = jetEta+m_etaHalfWidth;  
	
	TrigRoiDescriptor* roi =  new TrigRoiDescriptor(jetEta,   etaMinus, etaPlus, 
							jetPhi,   phiMinus, phiPlus);

	// for checking Et and eta of jets in hypos later
	TrigOperationalInfo* trigInfoJetEt = new TrigOperationalInfo();
	trigInfoJetEt->set("EFJetEt", jetEt);
	HLT::ErrorCode hltEtStatus = attachFeature(outputTE, trigInfoJetEt, "EFJetInfo"); 
	if (hltEtStatus != HLT::OK) {
	  msg() << MSG::ERROR << "Failed to attach TrigOperationalInfo (jet Et) as feature" << endreq;
	  return hltEtStatus;
	}
	
	TrigOperationalInfo* trigInfoJetEta = new TrigOperationalInfo();
	trigInfoJetEta->set("EFJetEta", jetEta);
	HLT::ErrorCode hltEtaStatus = attachFeature(outputTE, trigInfoJetEta, "EFJetInfo"); 
	if (hltEtaStatus != HLT::OK) {
	  msg() << MSG::ERROR << "Failed to attach TrigOperationalInfo (jet eta) as feature" << endreq;
	  return hltEtaStatus;
	}
	
	HLT::ErrorCode hltStatus = attachFeature(outputTE, roi, m_jetOutputKey);
	if ( hltStatus != HLT::OK ) {
	  msg() << MSG::ERROR << "Failed to attach TrigRoiDescriptor as feature " << *roi << endreq;
	  return hltStatus;
	}

	// Make a deep copy of the jet to attach
	xAOD::JetTrigAuxContainer trigJetTrigAuxContainer;
	xAOD::JetContainer* jc = new xAOD::JetContainer;
	jc->setStore(&trigJetTrigAuxContainer);
	jc->push_back ( new xAOD::Jet(**Jet) );

	// and attach that jet as a feature
	hltStatus = attachFeature(outputTE, jc, m_jetOutputKey); 
	if (hltStatus != HLT::OK) {
	  msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer (" << m_jetOutputKey 
		<< ") as feature jet with ET =  " << jetEt << " eta = " << jetEta << " and phi = " << jetPhi << endreq;
	  return hltStatus;
	}
	
      }
      else {
	if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet " << iJet << " close to a muon.  Discard for b-tagging." <<  endreq; 
      }
    }
  }

  afterExecMonitors().ignore();

  return HLT::OK;  
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigFarawayJetFinderAllTE::hltFinalize() {

  msg() << MSG::INFO << "Finalizing TrigFarawayJetFinderAllTE" << endreq;

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigFarawayJetFinderAllTE::hltEndEvent() {

  if ( !AllTEAlgo::hltEndEvent() ) return false;
  AllTEAlgo::hltEndEvent();

  return true;
}

// -----------------------------------------------------------------------------------------------------------------

