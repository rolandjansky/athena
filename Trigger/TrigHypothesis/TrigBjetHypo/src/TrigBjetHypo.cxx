/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetHypo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigSteeringEvent/TrigPassBits.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"


//* ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetHypo::TrigBjetHypo(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator),
  m_cutCounter(0)
{
  declareProperty ("JetKey",    m_jetKey     = ""   ); //"EFJet" or "" needed for default config, "SplitJet" for new config
  declareProperty ("AcceptAll", m_acceptAll         );
  declareProperty ("CutXIP2D",  m_xcutIP2D   = -20  );
  declareProperty ("CutXIP3D",  m_xcutIP3D   = -20  );
  declareProperty ("CutXCOMB",  m_xcutCOMB   = -20  );
  declareProperty ("CutXCHI2",  m_xcutCHI2   = -20  );
  declareProperty ("CutMV2c20", m_xcutMV2c20 = -20  );
  declareProperty ("MethodTag", m_methodTag  = ""   );
  declareProperty ("Instance",  m_instance          );

  declareProperty ("UseBeamSpotFlag", m_useBeamSpotFlag = false);

  declareMonitoredVariable("Method", m_monitorMethod);
  declareMonitoredVariable("CutCounter", m_cutCounter);
}


// -----------------------------------------------------------------------------------------------------------------


TrigBjetHypo::~TrigBjetHypo() {}


// -----------------------------------------------------------------------------------------------------------------


HLT::ErrorCode TrigBjetHypo::hltInitialize() {

  // Get message service 
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigBjetHypo" << endreq;   

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " AcceptAll = "       << m_acceptAll       << endreq; 
    msg() << MSG::DEBUG << " MethodTag = "       << m_methodTag       << endreq; 
    msg() << MSG::DEBUG << " Instance = "        << m_instance        << endreq; 
    msg() << MSG::DEBUG << " UseBeamSpotFlag = " << m_useBeamSpotFlag << endreq; 
    msg() << MSG::DEBUG << " JetKey = "          << m_jetKey          << endreq; 

    if (m_xcutMV2c20 != -20) msg() << MSG::DEBUG << " CutMV2c20 = "  << m_xcutMV2c20 << endreq; 
    if (m_xcutCOMB   != -20) msg() << MSG::DEBUG << " CutXCOMB  = "  << m_xcutCOMB   << endreq; 
    if (m_xcutCHI2   != -20) msg() << MSG::DEBUG << " CutXCHI2  = "  << m_xcutCHI2   << endreq; 
    if (m_xcutIP3D   != -20) msg() << MSG::DEBUG << " CutXIP3D  = "  << m_xcutIP3D   << endreq; 
    if (m_xcutIP2D   != -20) msg() << MSG::DEBUG << " CutXIP2D  = "  << m_xcutIP2D   << endreq; 
  }

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigBjetHypo" << endreq;

  // AcceptAll declare property setting 
  if (m_acceptAll) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: AcceptAll property is set: taking all events and applying the selection only for saving the TrigPassBits" << endreq;
  } else 
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: AcceptAll property not set: applying the selection and saving the TrigPassBits" << endreq;
  
  // initialise monitoring variables 
  m_cutCounter = -1;

  // Retrieve beamspot information 
  if (m_useBeamSpotFlag) {

    IBeamCondSvc* m_iBeamCondSvc; 
    StatusCode sc = service("BeamCondSvc", m_iBeamCondSvc);
    
    if (sc.isFailure() || m_iBeamCondSvc == 0) {
      
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endreq;
      
    } else {

      int m_beamSpotStatus = 0;
      int m_beamSpotBitMap = m_iBeamCondSvc->beamStatus();    

      m_beamSpotStatus = ((m_beamSpotBitMap & 0x4) == 0x4);  
      if (m_beamSpotStatus) m_beamSpotStatus = ((m_beamSpotBitMap & 0x3) == 0x3);
      
      if (!m_beamSpotStatus) {
	
	m_cutCounter=0;
	pass = false;
	
	return HLT::OK;
      }
    }
  }

  // Get RoI descriptor 
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor, m_jetKey);
  
  if (stat == HLT::OK) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Using outputTE: " << *roiDescriptor << endreq;
      //<< "RoI id " << roiDescriptor->roiId()
      //    << ", Phi = " <<  roiDescriptor->phi()
      //	    << ", Eta = " << roiDescriptor->eta() << endreq;
    }
  } else {
    if (msgLvl() <= MSG::WARNING) 
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endreq;

    return HLT::NAV_ERROR;
  }

  bool result = false;

  // Define TrigPassBits for b-jets 
  TrigPassBits *bitsEF=0;

  // Retrieve xAOD b-jet object 
  const xAOD::BTaggingContainer* trigBTaggingContainer=0;
  if(getFeature(outputTE, trigBTaggingContainer, "HLTBjetFex") != HLT::OK) {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to get BTaggingContainer" << endreq;
    return HLT::OK;
  }
  if(msgLvl() <= MSG::DEBUG && trigBTaggingContainer != 0) {
    msg() << MSG::DEBUG << "Got BTaggingContainer with " << trigBTaggingContainer->size() << " BTagging object" << endreq;
  }
  if(trigBTaggingContainer->size() > 1) {
    if(msgLvl() <= MSG::ERROR)
      msg() << MSG::ERROR << "More than one BTagging object to analyse: this should never happen" << endreq;
    return HLT::NAV_ERROR;
  }
  if(trigBTaggingContainer->size() == 0) {
    if(msgLvl() <= MSG::ERROR)
      msg() << MSG::ERROR << "No BTagging object to analyse: this should never happen" << endreq;
    return HLT::NAV_ERROR;
  }

  // Add TrigPassBits for EF b-jets 
  bitsEF = HLT::makeTrigPassBits(trigBTaggingContainer);
  
  // to separate bad input TE and true behaviour 
  m_cutCounter=1;
  
  auto trigBTagging = trigBTaggingContainer->begin();
  auto trigBTaggingEnd = trigBTaggingContainer->end();

  // Loop over EFBjets and perform cut 
  if (m_methodTag == "MV2c20") {

    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) { 

      double x = (*trigBTagging)->auxdata<double>("MV2c20_discriminant");

      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "MV2c20 x =  " << x;
      if(x>m_xcutMV2c20) {
	HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endreq;
	result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endreq;
      }
    }
  } 
  else if (m_methodTag == "COMB") {
    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) { 
      double w=((*trigBTagging)->IP3D_pb()/(*trigBTagging)->IP3D_pu()) * ((*trigBTagging)->SV1_pb()/(*trigBTagging)->SV1_pu());
      double x=50;
      if(w/(1+w)<1) x=-1.0*TMath::Log10(1-(w/(1+w)));
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "COMB x =  " << x;
      if(x>m_xcutCOMB) {
	HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endreq;
	result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endreq;
      }
    }
  } 
  else if (m_methodTag == "IP3D") {
    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) { 
      double w=(*trigBTagging)->IP3D_pb()/(*trigBTagging)->IP3D_pu();
      double x=-40;
      if(w>0) x=TMath::Log10(w);
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "IP3D x =  " << x;
      if(x>m_xcutIP3D) {
	HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endreq;
	result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endreq;
      }
    }
  } 
  else if (m_methodTag == "IP2D") {
    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) { 
      double w=(*trigBTagging)->IP2D_pb()/(*trigBTagging)->IP2D_pu();
      double x=-40;
      if(w>0) x=TMath::Log10(w);
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "IP2D x =  " << x;
      if(x>m_xcutIP2D) {
	HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endreq;
	result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endreq;
      }
    }
  }

  if(result) m_cutCounter++;
  pass = result;
  
  // Print trigger decision 
  if (m_acceptAll) {
    if(msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: Trigger decision is 1" << endreq;
  } else {
    if(msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: Trigger decision is " << pass << endreq;
  }

  // Monitoring of method used to perform the cut 
  if (m_methodTag == "IP2D") m_monitorMethod = 1;
  else if (m_methodTag == "IP3D") m_monitorMethod = 2;
  else if (m_methodTag == "COMB") m_monitorMethod = 3;

  // Print TrigPassBits to outputTE 
  if (attachBits(outputTE, bitsEF, "EFBjets") != HLT::OK) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits for b-jets" << endreq;
  }
 
  if (m_acceptAll) {
    m_cutCounter = 2;
    pass = true;
    return HLT::OK;
  }

  return HLT::OK;
}


// -----------------------------------------------------------------------------------------------------------------


HLT::ErrorCode TrigBjetHypo::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetHypo" << endreq;

  return HLT::OK;
}

