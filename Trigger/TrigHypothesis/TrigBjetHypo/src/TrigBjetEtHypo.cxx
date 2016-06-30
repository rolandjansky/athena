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
#include "TrigSteeringEvent/TrigOperationalInfo.h"

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
    msg() << MSG::INFO << "Initializing TrigBjetEtHypo" << endreq;   

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " AcceptAll = "      << m_acceptAll   << endreq;
    msg() << MSG::DEBUG << " EtThreshold = "    << m_etThreshold << endreq; 
    msg() << MSG::DEBUG << " Instance = "       << m_instance    << endreq;
    msg() << MSG::DEBUG << " Version = "        << m_version     << endreq;
    msg() << MSG::DEBUG << " JetKey = "         << m_jetKey      << endreq;
  }

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetEtHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigBjetEtHypo in its version " << m_version << endreq;



//  //* Get RoI descriptor *//
//  const TrigRoiDescriptor* roiDescriptor = 0;
//  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor, "");
//  
////  if (stat == HLT::OK) {
//    if (msgLvl() <= MSG::DEBUG) {
//      msg() << MSG::DEBUG << "Using outputTE: ROI: " << &roiDescriptor << endreq;
//    }
//  } 
//  else {
//    if (msgLvl() <= MSG::WARNING) 
//      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endreq;
//    
//    //return HLT::NAV_ERROR;
//  }
//
  
  // Get TrigOperationalInfo 
  std::vector<const TrigOperationalInfo*> m_vectorOperationalInfo;
  if (getFeatures(outputTE, m_vectorOperationalInfo, "EFJetInfo") != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigOperationalInfo" << endreq;
    }
    return HLT::MISSING_FEATURE;
  } 
  else {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Number of TrigOperationalInfo objects: " << m_vectorOperationalInfo.size() << endreq;
    }
  }

  pass = false;

  const xAOD::JetContainer* jets(0);
  HLT::ErrorCode ec = getFeature(outputTE, jets, m_jetKey);
    
  if(ec!=HLT::OK) {
    ATH_MSG_WARNING("Failed to get JetCollection");
    return ec;
  } else {
    ATH_MSG_DEBUG("Obtained JetContainer");
  }
    
  msg() << MSG::DEBUG << "pass Hypo  " << &jets << endreq;
    
  if(jets == 0){
    ATH_MSG_WARNING("Jet collection pointer is 0");
    return HLT::ERROR;
  }
    
  std::vector<const xAOD::Jet*> theJets(jets->begin(), jets->end());
    
  msg() << MSG::DEBUG << "pass jet size: " << theJets.size() << endreq;
    

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
  msg() << MSG::DEBUG << "in Hypo pt  " << theJets.at(0)->p4().Pt() << " and eta " << theJets.at(0)->p4().Eta() << endreq;
    
  if (msgLvl() <= MSG::DEBUG){
    double etajet = theJets.at(0)->p4().Eta();  
    msg() << MSG::DEBUG << "in Hypo et  " << m_et << " and eta " << etajet << endreq;
  }
    
      
  if (m_acceptAll) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST: AcceptAll property is set: taking all events" << endreq;
      msg() << MSG::DEBUG << "REGTEST: Trigger decision is 1" << endreq;
    }
    pass = true;
  } else {
    
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: EF jet with et = " << m_et << endreq;
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Requiring EF jets to satisfy Et > " << m_etThreshold << endreq;

    msg() << MSG::DEBUG << "REGTEST: EF jet with et = " << m_et << endreq;
    msg() << MSG::DEBUG << "REGTEST: Requiring EF jets to satisfy Et > " << m_etThreshold << endreq;
    
    if (m_et >= m_etThreshold)
      pass = true;
    msg() << MSG::DEBUG << "REGTEST: Pass " << pass << endreq;
  }

  if (pass) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Selection cut satisfied, accepting the event" << endreq;
  } 
  else {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Selection cut not satisfied, rejecting the event" << endreq;
  }
  
  //* Print trigger decision *//
  if(msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "REGTEST: Trigger decision is " << pass << endreq;


  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetEtHypo::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetEtHypo" << endreq;

  return HLT::OK;
}

