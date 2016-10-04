/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetEtHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetEtHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetEtHypo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
// #include "xAODTracking/VertexContainer.h"
// #include "xAODTracking/VertexAuxContainer.h"
// #include "xAODBTagging/BTagVertexContainer.h"
// #include "xAODBTagging/BTagVertexAuxContainer.h"


// ----------------------------------------------------------------------------------------------------------------- 


TrigBjetEtHypo::TrigBjetEtHypo(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator),
  m_etThreshold(0),
  m_et(0)
{
  declareProperty ("AcceptAll",   m_acceptAll);
  declareProperty ("EtThreshold", m_etThreshold);
  declareProperty ("Instance",    m_instance);
  declareProperty ("Version",     m_version);
  declareProperty ("JetKey",      m_jetKey  = ""); //"" needed for default config, SplitJet for new config, FarawayJet for muon-jet chains with b-tagging
}


// ----------------------------------------------------------------------------------------------------------------- 


TrigBjetEtHypo::~TrigBjetEtHypo() {}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetEtHypo::hltInitialize() {

  //* Get message service *//
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigBjetEtHypo" << endmsg;   

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endmsg;
    msg() << MSG::DEBUG << " AcceptAll = "      << m_acceptAll   << endmsg;
    msg() << MSG::DEBUG << " EtThreshold = "    << m_etThreshold << endmsg; 
    msg() << MSG::DEBUG << " Instance = "       << m_instance    << endmsg;
    msg() << MSG::DEBUG << " Version = "        << m_version     << endmsg;
    msg() << MSG::DEBUG << " JetKey = "         << m_jetKey      << endmsg;
  }

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetEtHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigBjetEtHypo in its version " << m_version << endmsg;

  const xAOD::JetContainer* jets(0);
  HLT::ErrorCode ec = getFeature(outputTE, jets, m_jetKey);
    
  if(ec!=HLT::OK) {
    ATH_MSG_WARNING("Failed to get JetCollection");
    return ec;
  } else {
    ATH_MSG_DEBUG("Obtained JetContainer");
  }
    
  msg() << MSG::DEBUG << "pass Hypo  " << &jets << endmsg;
  
  pass = false;
  
  if(jets == 0){
    ATH_MSG_WARNING("Jet collection pointer is 0");
    return HLT::ERROR;
  }
    
  std::vector<const xAOD::Jet*> theJets(jets->begin(), jets->end());
    
  msg() << MSG::DEBUG << "pass jet size: " << theJets.size() << endmsg;
    

  std::size_t njets = theJets.size();
  if( njets == 0 ){
    ATH_MSG_DEBUG("JetCollection is empty");
    return HLT::OK;
  } 
  else {
    ATH_MSG_DEBUG("JetCollection contains " << njets << " jets");
  }
  if(njets > 1){
    ATH_MSG_WARNING("Something is wrong, it should not be more than one jet");
    return HLT::OK;
  }

  double m_et = theJets.at(0)->p4().Et();
  msg() << MSG::DEBUG << "in Hypo pt  " << theJets.at(0)->p4().Pt() << " and eta " << theJets.at(0)->p4().Eta() << endmsg;
    
  if (msgLvl() <= MSG::DEBUG){
    double etajet = theJets.at(0)->p4().Eta();  
    msg() << MSG::DEBUG << "in Hypo et  " << m_et << " and eta " << etajet << endmsg;
  }
    
      
  if (m_acceptAll) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST: AcceptAll property is set: taking all events" << endmsg;
      msg() << MSG::DEBUG << "REGTEST: Trigger decision is 1" << endmsg;
    }
    pass = true;
  } else {
    
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: EF jet with et = " << m_et << endmsg;
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Requiring EF jets to satisfy Et > " << m_etThreshold << endmsg;

    msg() << MSG::DEBUG << "REGTEST: EF jet with et = " << m_et << endmsg;
    msg() << MSG::DEBUG << "REGTEST: Requiring EF jets to satisfy Et > " << m_etThreshold << endmsg;
    
    if (m_et >= m_etThreshold)
      pass = true;
    msg() << MSG::DEBUG << "REGTEST: Pass " << pass << endmsg;
  }

  if (pass) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Selection cut satisfied, accepting the event" << endmsg;
  } 
  else {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Selection cut not satisfied, rejecting the event" << endmsg;
  }
  
  //* Print trigger decision *//
  if(msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "REGTEST: Trigger decision is " << pass << endmsg;


  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetEtHypo::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetEtHypo" << endmsg;

  return HLT::OK;
}

