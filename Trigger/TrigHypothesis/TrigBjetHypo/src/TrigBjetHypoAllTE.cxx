/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypoAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHORS:   John Alison   johnda@uchicago.edu
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetHypoAllTE.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"


TrigBjetHypoAllTE::TrigBjetHypoAllTE(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator),
  m_cutCounter(0)
{
  declareProperty("BTaggingKey", m_btaggingKey      = "");
  declareProperty ("UseBeamSpotFlag",       m_useBeamSpotFlag       = false);
  declareProperty ("OverrideBeamSpotValid", m_overRideBeamSpotValid = false);

  declareMonitoredVariable("CutCounter",   m_cutCounter);

  //declareMonitoredVariable("DeltaRPass",    m_deltaRPass);
  //declareMonitoredVariable("DeltaRAll",    m_deltaRAll);
  //declareMonitoredVariable("DeltaZPass",    m_deltaZPass);
  //declareMonitoredVariable("DeltaZAll",    m_deltaZAll);

}


//
// Destruction
//
TrigBjetHypoAllTE::~TrigBjetHypoAllTE() {}


//
// hltInitialize
//
HLT::ErrorCode TrigBjetHypoAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO) msg() << MSG::INFO << "Initializing TrigBjetHypoAllTE" << endmsg;
    
  //
  // declareProperty overview
  //
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endmsg;
    msg() << MSG::DEBUG << " BTaggingKey = "   << m_btaggingKey << endmsg;
  }
  
  return HLT::OK;
}


//
//  hltExecute
//
HLT::ErrorCode TrigBjetHypoAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {
  std::cout << "Enter TrigBjetHypoAllTE " << std::endl;  
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigBjetHypoAllTE" << endmsg;

  beforeExecMonitors().ignore();

  m_cutCounter   = -1;

  bool pass = false;

  //
  // Retrieve beamspot information 
  //
  if (m_useBeamSpotFlag) {

    IBeamCondSvc* iBeamCondSvc; 
    StatusCode sc = service("BeamCondSvc", iBeamCondSvc);
    
    if (sc.isFailure() || iBeamCondSvc == 0) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endmsg;
    } else {

      int beamSpotStatus = 0;
      int beamSpotBitMap = iBeamCondSvc->beamStatus();    

      beamSpotStatus = ((beamSpotBitMap & 0x4) == 0x4);  
      if (beamSpotStatus) beamSpotStatus = ((beamSpotBitMap & 0x3) == 0x3);
      
      if (!beamSpotStatus && !m_overRideBeamSpotValid) {
	m_cutCounter=0;
	pass = false;
	return HLT::OK;
      }
    }
  }


  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
				    << "Number of input TEs is " <<  inputTE.size() 
				    << " TE0: " << inputTE[0].size() 
				    << " TE1: " << inputTE[1].size() <<  endmsg;  


  if (inputTE.size() < 1) {
    msg() << MSG::WARNING << "Number of input TEs is " <<  inputTE.size() << " and not 1. Configuration problem." << endmsg;  
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  if (inputTE.size() > 2) {
    msg() << MSG::WARNING << "Too many TEs passed as input" << endmsg;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }


  //
  // Retrieve the BTagging container
  //
  std::vector<HLT::TriggerElement*>& btaggingTEs = inputTE.at(0); 

  if (btaggingTEs.size() == 0) {
    msg() << MSG::WARNING << "Got an empty inputTE (btagging)" << endmsg;
    afterExecMonitors().ignore();
    return HLT::MISSING_FEATURE; 
  }

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " btaggingTE.size() " << btaggingTEs.size() << endmsg;

  const xAOD::BTaggingContainer* trigBTaggingContainer=0;
  HLT::ErrorCode statusBTagging = getFeature(btaggingTEs.front(), trigBTaggingContainer, m_btaggingKey); 

  if (statusBTagging != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (btagging)" << endmsg;
    return HLT::NAV_ERROR;
  }

  if(trigBTaggingContainer==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (btagging)." << endmsg;
    return HLT::MISSING_FEATURE;
  }

  std::cout << "Found " << trigBTaggingContainer->size() << " btagging objsects " << std::endl;;

  //
  //  Do the multiplicity cut
  //
  std::cout << "In TrigBjetHypoAllTE " << std::endl;
  for(xAOD::BTagging* trigBJet : *trigBTaggingContainer){
    double btagWeight = trigBJet->auxdata<double>("MV2c20_discriminant");  
    std::vector< ElementLink< xAOD::IParticleContainer > >  matchedJets = trigBJet->auxdata< std::vector< ElementLink< xAOD::IParticleContainer > > >("BTagBtagToJetAssociator");  
    std::cout << btagWeight << " " << matchedJets.size() << std::endl;
  }

  //
  // Testing
  //
  unsigned int nOver35 = 0;

  for(HLT::TriggerElement* btagTE : inputTE.at(0)){
    const xAOD::BTaggingContainer* thisBTaggingContainer=0;
    HLT::ErrorCode thisStatusBTagging = getFeature(btagTE, thisBTaggingContainer, m_btaggingKey); 

    if (thisStatusBTagging != HLT::OK) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (btagging)" << endmsg;
      return HLT::NAV_ERROR;
    }

    if(thisBTaggingContainer==0) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (btagging)." << endmsg;
      return HLT::MISSING_FEATURE;
    }

    std::cout << "Found " << thisBTaggingContainer->size() << " btagging objsects " << std::endl;;

    std::cout << "In TrigBjetHypoAllTE round2 " << std::endl;
    for(xAOD::BTagging* trigBJet : *thisBTaggingContainer){
      double btagWeight = trigBJet->auxdata<double>("MV2c20_discriminant");  
      std::vector< ElementLink< xAOD::IParticleContainer > >  matchedJets = trigBJet->auxdata< std::vector< ElementLink< xAOD::IParticleContainer > > >("BTagBtagToJetAssociator");  
      std::cout << btagWeight << " " << matchedJets.size() << std::endl;
      
      if(matchedJets.size() != 1){
	if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jet element link) size=" << matchedJets.size() << endmsg;
	return HLT::MISSING_FEATURE;
      }

      const xAOD::IParticleContainer* jetCol = matchedJets.at(0).getStorableObjectPointer();
      std::cout << "jetCol.size() " << jetCol->size() << std::endl;
      
      if(jetCol->size() != 1){
      	if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jetCollection) size=" << jetCol->size() << endmsg;
      	return HLT::MISSING_FEATURE;
      }
      
      const xAOD::JetContainer* jets = dynamic_cast<const xAOD::JetContainer*>(jetCol);

      if(jets->size() != 1){
	if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jets) size=" << jets->size() << endmsg;
	return HLT::MISSING_FEATURE;
      }

      std::cout << jets->at(0)->p4().Et()*0.001 << " " << jets->at(0)->eta() << " " << jets->at(0)->phi() << std::endl;
      if(jets->at(0)->p4().Et() > 35*1000)
	++nOver35;
    }
    
  }//inputTE.at(0)
      
  if(nOver35 > 3) pass = true;
  
  if (pass) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Accepting the event" << endmsg;
    m_cutCounter=1;
  } else {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Rejecting the event" << endmsg;
    m_cutCounter=0;
  }
  

  //
  // Setting the output TE
  //
  HLT::TEVec allTEs;
  if ((inputTE.size()>0) && (inputTE[0].size()>0)) 
    allTEs.push_back(inputTE[0][0]);

  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
  outputTE->setActiveState(false); 

  if (pass==true){
    outputTE->setActiveState(true);
  }
 
  afterExecMonitors().ignore();

  return HLT::OK;  
}



//
// hltFinalize
//
HLT::ErrorCode TrigBjetHypoAllTE::hltFinalize() {

  msg() << MSG::INFO << "Finalizing TrigBjetHypoAllTE" << endmsg;

  return HLT::OK;
}


//
// hltEndEvent
//
HLT::ErrorCode TrigBjetHypoAllTE::hltEndEvent() {

  if ( !AllTEAlgo::hltEndEvent() ) return false;
  AllTEAlgo::hltEndEvent();

  return true;
}



