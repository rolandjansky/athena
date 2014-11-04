// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   @file    TrigSuperRoiBuilderAllTE.cxx        
//
//            Creates a super ROI from a jet collection
//                   
//  
//   Katharine (dot) Leney (at cern dot ch)
//
//   20th October 2014


#include "GaudiKernel/IToolSvc.h"

#include "TrigBjetHypo/TrigSuperRoiBuilderAllTE.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigSuperRoi.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigNavigation/TriggerElement.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "xAODJet/JetContainer.h"
#include "AthContainers/ConstDataVector.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigSuperRoiBuilderAllTE::TrigSuperRoiBuilderAllTE(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty ("JetInputKey",  m_jetInputKey  = "TrigJetRec");
  declareProperty ("JetOutputKey", m_jetOutputKey = "TrigJetRec");
  declareProperty ("EtaHalfWidth", m_etaHalfWidth = 0.2);
  declareProperty ("PhiHalfWidth", m_phiHalfWidth = 0.2);
  declareProperty ("JetMinEt",     m_minJetEt     = 15.0); // in GeV
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigSuperRoiBuilderAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigSuperRoiBuilderAllTE, version " << PACKAGE_VERSION << endreq;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " JetInputKey = "  << m_jetInputKey  << endreq; 
    msg() << MSG::DEBUG << " JetOutputKey = " << m_jetOutputKey << endreq; 
    msg() << MSG::DEBUG << " EtaHalfWidth = " << m_etaHalfWidth << endreq; 
    msg() << MSG::DEBUG << " PhiHalfWidth = " << m_phiHalfWidth << endreq; 
    msg() << MSG::DEBUG << " MinJetEt     = " << m_minJetEt     << endreq; 
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigSuperRoiBuilderAllTE::~TrigSuperRoiBuilderAllTE(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigSuperRoiBuilderAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Running TrigSuperRoiBuilderAllTE::hltExecute" << endreq;

  beforeExecMonitors().ignore();

  if (inputTEs.size() != 1) {
    msg() << MSG::WARNING << "Got more than one inputTE" << endreq;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  std::vector<HLT::TriggerElement*>& inputTE = inputTEs.at(0);

  if (inputTE.size() == 0) {
    msg() << MSG::WARNING << "Got an empty inputTE" << endreq;
    afterExecMonitors().ignore();
    return HLT::MISSING_FEATURE; 
  }

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " inputTEs.size() " << inputTEs.size() << " inputTE.size() " << inputTE.size() << endreq;


  // xAOD conversion const JetCollection* outJets(0);
  const xAOD::JetContainer* jets = 0;
  //HLT::ErrorCode statusJets = getFeature(inputTE.front(), jets, m_jetInputKey);
  HLT::ErrorCode statusJets = getFeature(inputTE.front(), jets); // this should really be given a name - need to find out froim the jet guys what!
  
  if (statusJets != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features" << endreq;
    return HLT::NAV_ERROR;
  } 

  if(jets==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature." << endreq;
    return HLT::MISSING_FEATURE;
  }

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Found " << jets->size() << " jets, creating corresponding RoIs" << endreq; 

  // Create a superROI to add all the jet ROIs to.
  TrigRoiDescriptor* superRoi = new TrigRoiDescriptor();
  superRoi->setComposite(true);

  HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
  HLT::TriggerElement* outputTE  = config()->getNavigation()->addNode(initialTE,output);
  outputTE->setActiveState(true);
  std::string key = "";

  unsigned int i = 0;
  for ( xAOD::JetContainer::const_iterator jetitr=jets->begin() ; jetitr!=jets->end() ; jetitr++ ) { 
  
    ++i;
    auto jet = *jetitr;

    float jetEt  = jet->p4().Et()*0.001;
    float jetEta = jet->eta();
    float jetPhi = jet->phi();

    if (jetEt < m_minJetEt) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Jet "<< i << " below the " << m_minJetEt << " GeV threshold; Et " << jetEt << "; skipping this jet." << endreq;
      continue;
    }

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Jet "<< i << "; Et " << jetEt << "; eta "<< jetEta << "; phi " << jetPhi << endreq;

    // create RoI correspondinding to the jet
    double phiMinus = HLT::wrapPhi(jetPhi-m_phiHalfWidth); 
    double phiPlus  = HLT::wrapPhi(jetPhi+m_phiHalfWidth); 
    double etaMinus = jetEta-m_etaHalfWidth;  
    double etaPlus  = jetEta+m_etaHalfWidth;  

    TrigRoiDescriptor* roi =  new TrigRoiDescriptor(jetEta, etaMinus, etaPlus, 
						    jetPhi, phiMinus, phiPlus );

    msg() << MSG::DEBUG << "Adding ROI descriptor to superROI!" << endreq;
    superRoi->push_back( roi );

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

  }

  msg() << MSG::DEBUG << "Attaching feature" << endreq;
  HLT::ErrorCode hltStatus = attachFeature(outputTE, superRoi, m_jetOutputKey);
  if (hltStatus != HLT::OK) {
    msg() << MSG::ERROR << "Failed to attach SuperRoi as feature" << endreq;
    return hltStatus;
  }
  
  // Check that we created the superRoi correctly and can retreive it
  // This is really just a sanity check - can probably be removed...
  const TrigSuperRoi* newSuperRoi;
  if (getFeature(outputTE, newSuperRoi) != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) 
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endreq;
    
    return HLT::NAV_ERROR;
  }

  afterExecMonitors().ignore();

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigSuperRoiBuilderAllTE::hltFinalize() {

  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}



