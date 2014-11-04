/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetEtaHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetEtaHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetEtaHypo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetEtaHypo::TrigBjetEtaHypo(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator),
  m_etaMax(0),
  m_eta(0)
{
  declareProperty ("AcceptAll", m_acceptAll);
  declareProperty ("EtaMax",    m_etaMax);
  declareProperty ("Instance",  m_instance);
  declareProperty ("Version",   m_version);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetEtaHypo::~TrigBjetEtaHypo() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetEtaHypo::hltInitialize() {

  //* Get message service *//
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigBjetEtaHypo" << endreq;   

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " AcceptAll = "  << m_acceptAll << endreq;
    msg() << MSG::DEBUG << " EtaMax    = "  << m_etaMax    << endreq; 
    msg() << MSG::DEBUG << " Instance  = "  << m_instance  << endreq;
    msg() << MSG::DEBUG << " Version   = "  << m_version   << endreq;
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetEtaHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigBjetEtaHypo in its version " << m_version << endreq;

  //* Get RoI descriptor *//
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor, "");
  
  if (stat != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) 
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endreq;   
    return HLT::NAV_ERROR;
  }
  
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

  // Loop over TrigOperationalInfo vector to get eta of each of the jets

  std::vector<const TrigOperationalInfo*>::const_iterator m_operationalInfo;
  for (m_operationalInfo=m_vectorOperationalInfo.begin(); m_operationalInfo!=m_vectorOperationalInfo.end(); ++m_operationalInfo) {    

    if ((*m_operationalInfo)->defined("HLTJetEta")==1) {
    
      unsigned int m_etSize = (*m_operationalInfo)->infos().first.size();    
      if (m_etSize!=1) {
	if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "More than one Eta threshold associated to the same HLT jet" << endreq;
	return HLT::NAV_ERROR;
      }
    
      m_eta = (*m_operationalInfo)->get("HLTJetEta");
    
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet eta: " << m_eta << endreq;
    }
  }


  // Presumably can just use the ROI eta/phi here? 
  // Or do I need to attach the jet eta as TrigOperationalInfo in JetSplitter/SuperROI builder?

  pass = false;
  
  
  // Make the eta cut here:
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Requiring HLT jets to satisfy |eta| < " << m_etaMax << endreq;
  
  if (fabs(m_eta) <= m_etaMax)  pass = true;
    
  if (pass) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Selection cut satisfied, accepting the event" << endreq;
  } else {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Selection cut not satisfied, rejecting the event" << endreq;
  }

  //* Print trigger decision *//
  if(msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "REGTEST: Trigger decision is " << pass << endreq;
    
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetEtaHypo::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetEtaHypo" << endreq;

  return HLT::OK;
}

