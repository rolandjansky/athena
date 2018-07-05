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
  m_etThreshold(0)
  //  m_et(0)
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

  ATH_MSG_DEBUG( "declareProperty review:" );
  ATH_MSG_DEBUG( " AcceptAll = "      << m_acceptAll );
  ATH_MSG_DEBUG( " EtThreshold = "    << m_etThreshold );
  ATH_MSG_DEBUG( " Instance = "       << m_instance );
  ATH_MSG_DEBUG( " Version = "        << m_version );
  ATH_MSG_DEBUG( " JetKey = "         << m_jetKey );

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetEtHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  ATH_MSG_DEBUG( "Executing TrigBjetEtHypo in its version " << m_version );

  const xAOD::JetContainer* jets(0);
  HLT::ErrorCode ec = getFeature(outputTE, jets, m_jetKey);
    
  if(ec!=HLT::OK) {
    ATH_MSG_WARNING("Failed to get JetCollection");
    return ec;
  } else {
    ATH_MSG_DEBUG("Obtained JetContainer");
  }
    
  ATH_MSG_DEBUG( "pass Hypo  " << &jets );
  
  pass = false;
  
  if(jets == 0){
    ATH_MSG_WARNING("Jet collection pointer is 0");
    return HLT::ERROR;
  }
    
  std::vector<const xAOD::Jet*> theJets(jets->begin(), jets->end());
    
  ATH_MSG_DEBUG("pass jet size: " << theJets.size() );
    

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

  double et = theJets.at(0)->p4().Et();
  ATH_MSG_DEBUG( "in Hypo pt  " << theJets.at(0)->p4().Pt() << " and eta " << theJets.at(0)->p4().Eta() );
    
  if (msgLvl() <= MSG::DEBUG){
    double etajet = theJets.at(0)->p4().Eta();  
    ATH_MSG_DEBUG( "in Hypo et  " << et << " and eta " << etajet );
  }
    
      
  if (m_acceptAll) {
    if (msgLvl() <= MSG::DEBUG) {
      ATH_MSG_DEBUG( "REGTEST: AcceptAll property is set: taking all events" );
      ATH_MSG_DEBUG( "REGTEST: Trigger decision is 1" );
    }
    pass = true;
  } else {
    
    ATH_MSG_DEBUG( "REGTEST: EF jet with et = " << et );
    ATH_MSG_DEBUG( "REGTEST: Requiring EF jets to satisfy Et > " << m_etThreshold );
    
    if (et >= m_etThreshold)
      pass = true;

    ATH_MSG_DEBUG( "REGTEST: Pass " << pass );
  }
  
  if (pass) {
    ATH_MSG_DEBUG( "Selection cut satisfied, accepting the event" );
  } 
  else {
    ATH_MSG_DEBUG( "Selection cut not satisfied, rejecting the event" );
  }
  
  //* Print trigger decision *//
  ATH_MSG_DEBUG( "REGTEST: Trigger decision is " << pass );


  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetEtHypo::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetEtHypo" << endmsg;

  return HLT::OK;
}

