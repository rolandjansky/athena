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
  declareProperty ("Tagger",                m_tagger = "MV2c10_discriminant");
  declareMonitoredVariable("CutCounter",   m_cutCounter);

  declareProperty("EtThresholds",   m_EtThresholds   ); 
  declareProperty("BTagCuts",       m_BTagCuts       );
  declareProperty("Multiplicities", m_Multiplicities );

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

  unsigned int nRequirements = m_EtThresholds.size();

  if(nRequirements != m_BTagCuts.size()){
    msg() << MSG::ERROR << "Et and btagging requirement have different sizes! Please Fix" << endmsg;
    return HLT::ERROR;
  }

  if(nRequirements != m_Multiplicities.size()){
    msg() << MSG::ERROR << "Et and multiplicities requirement have different sizes! Please Fix" << endmsg;
    return HLT::ERROR;
  }

  for(unsigned int iReq = 0; iReq < nRequirements; ++iReq){
    m_triggerReqs.push_back(triggerRequirement(m_EtThresholds.at(iReq), m_BTagCuts.at(iReq), m_Multiplicities.at(iReq)));
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

  //
  // Do the multiplicity requrements
  //
  clearCounters();

  for(HLT::TriggerElement* btagTE : inputTE.at(0)){

    const xAOD::BTagging* btagInfo = getBTaggingPtr(btagTE);    
    if (!btagInfo) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (btagging)" << endmsg;
      return HLT::MISSING_FEATURE;
    }

    const xAOD::Jet* jet = getJetPtr(btagInfo);
    if(!jet) {
      if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jet)." << endmsg;
      return HLT::MISSING_FEATURE;
    }
    
    double btagWeight = btagInfo->auxdata<double>(m_tagger);  
    float  btagEt     = jet->p4().Et();

    for(triggerRequirement& trigReq:  m_triggerReqs){
      if(btagEt > trigReq.m_EtThreshold  && btagWeight > trigReq.m_btagCut)
	++trigReq.m_count;
    }

    //
    // Check if event passes after each jet
    //  If so, pass trigger and move on
    //
    if(eventPassesTrigger()){
      pass = true;
      break;
    }

  }
  
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
// Check event decision
//
bool TrigBjetHypoAllTE::eventPassesTrigger(){
  bool passAND = true;
  for(triggerRequirement& trigReq:  m_triggerReqs){
    if(trigReq.m_count < trigReq.m_multiplicity)
      passAND = false;
  }
  return passAND;
}

//
// Clear counters
//
void TrigBjetHypoAllTE::clearCounters(){

  for(triggerRequirement& trigReq:  m_triggerReqs){
    trigReq.m_count = 0;
  }

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



//
// Retrieve the btagging info
//
const xAOD::BTagging* TrigBjetHypoAllTE::getBTaggingPtr(HLT::TriggerElement* btagTE) {

  const xAOD::BTaggingContainer* bTaggingContainer=0;
  HLT::ErrorCode statusBTagging = getFeature(btagTE, bTaggingContainer, m_btaggingKey); 

  if (statusBTagging != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (btagging)" << endmsg;
    return nullptr;
  }

  if(bTaggingContainer==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (btagging)." << endmsg;
    return nullptr;
  }

  if(bTaggingContainer->size() != 1) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Error btagging container size " << bTaggingContainer->size() << endmsg;
    return nullptr;
  }

  return bTaggingContainer->at(0);
}

//
// Retrieve the jet info
//
const xAOD::Jet* TrigBjetHypoAllTE::getJetPtr(const xAOD::BTagging* btagInfo) {
  
  std::vector< ElementLink< xAOD::IParticleContainer > >  matchedJets = btagInfo->auxdata< std::vector< ElementLink< xAOD::IParticleContainer > > >("BTagBtagToJetAssociator");  
      
  if(matchedJets.size() != 1){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jet element link) size=" << matchedJets.size() << endmsg;
    return nullptr;
  }

  const xAOD::IParticleContainer* jetCol = matchedJets.at(0).getStorableObjectPointer();
  if(jetCol->size() != 1){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (IParticleContainer) size=" << jetCol->size() << endmsg;
    return nullptr;
  }
      
  const xAOD::JetContainer* jets = dynamic_cast<const xAOD::JetContainer*>(jetCol);
  if(!jets){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jetContainer) size=" << jets->size() << endmsg;
    return nullptr;
  }
    
  if(jets->size() != 1){
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jets) size=" << jets->size() << endmsg;
    return nullptr;
  }
  
  return jets->at(0);
}
