// emacs: this is -*- c++ -*-
//
//   @file    TrigJetSplitterAllTE.cxx        
//
//            Splits a jet collection into separate TEs
//                   
//  
//   Copyright (C) 2014 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigJetSplitterAllTE.cxx, v0.0   Tue 17 Jun 2014 03:26:44 CEST sutt $


#include "GaudiKernel/IToolSvc.h"

#include "TrigBjetHypo/TrigJetSplitterAllTE.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigNavigation/TriggerElement.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "xAODJet/JetContainer.h"
#include "AthContainers/ConstDataVector.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigJetSplitterAllTE::TrigJetSplitterAllTE(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty ("JetInputKey",  m_jetInputKey  = "TrigJetRec");
  declareProperty ("JetOutputKey", m_jetOutputKey = "TrigJetRec");
  declareProperty ("EtaHalfWidth", m_etaHalfWidth = 0.2);
  declareProperty ("PhiHalfWidth", m_phiHalfWidth = 0.2);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigJetSplitterAllTE, version " << PACKAGE_VERSION << endreq;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " JetInputKey = "  << m_jetInputKey  << endreq; 
    msg() << MSG::DEBUG << " JetOutputKey = " << m_jetOutputKey << endreq; 
    msg() << MSG::DEBUG << " EtaHalfWidth = " << m_etaHalfWidth << endreq; 
    msg() << MSG::DEBUG << " PhiHalfWidth = " << m_phiHalfWidth << endreq; 
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigJetSplitterAllTE::~TrigJetSplitterAllTE(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Running TrigJetSplitterAllTE::hltExecute" << endreq;

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
  HLT::ErrorCode statusJets = getFeature(inputTE.front(), jets, m_jetInputKey);
  
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


  HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
  

  for ( xAOD::JetContainer::const_iterator jetitr=jets->begin() ; jetitr!=jets->end() ; jetitr++ ) { 
  
    auto jet = *jetitr;

    // Create an output TE seeded by an empty vector
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode( initialTE, output );
    outputTE->setActiveState(true);

    /// create RoI correspondinding to the jet
    double phiMinus = HLT::wrapPhi(jet->phi()-m_phiHalfWidth); 
    double phiPlus  = HLT::wrapPhi(jet->phi()+m_phiHalfWidth); 

    double etaMinus = jet->eta()-m_etaHalfWidth;  
    double etaPlus  = jet->eta()+m_etaHalfWidth;  

    TrigRoiDescriptor* roi =  new TrigRoiDescriptor(jet->eta(), etaMinus, etaPlus, 
						    jet->phi(), phiMinus, phiPlus );
    
    HLT::ErrorCode hltStatus = attachFeature(outputTE, roi, m_jetOutputKey);
    if ( hltStatus != HLT::OK ) {
      msg() << MSG::ERROR << "Failed to attach TrigRoiDescriptor as feature " << *roi << endreq;
      return hltStatus;
    }

    /// create jet container with this single jet 
    // xAOD::JetContainer* jc = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
    // xAOD::JetContainer* jc = new xAOD::JetContainer( jetitr, jetitr+1, SG::VIEW_ELEMENTS);

    ConstDataVector<xAOD::JetContainer>* jc = new ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
    jc->push_back (*jetitr);
  
    hltStatus = attachFeature(outputTE, jc, "EFJet"); 
    if (hltStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer as feature jet eta, phi " << jet->eta() << ", " << jet->phi() << endreq;
      return hltStatus;
    }
  }

  afterExecMonitors().ignore();

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitterAllTE::hltFinalize() {

  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}



