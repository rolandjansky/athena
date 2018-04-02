/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHORs:   John Alison    johnda@uchicago.edu
//            Andrea Coccaro Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetHypo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODTrigger/TrigPassBits.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"


//* ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetHypo::TrigBjetHypo(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator),
  m_cutCounter(0)
{
  declareProperty ("JetKey"          , m_jetKey     = ""          ); //"EFJet" or "" needed for default config, "SplitJet" for new config
  declareProperty ("AcceptAll"       , m_acceptAll                );
  declareProperty ("CutXIP2D"        , m_xcutIP2D   = -20         );
  declareProperty ("CutXIP3D"        , m_xcutIP3D   = -20         );
  declareProperty ("CutXCOMB"        , m_xcutCOMB   = -20         );
  declareProperty ("CutXCHI2"        , m_xcutCHI2   = -20         );
  declareProperty ("CutMV2c20"       , m_xcutMV2c20 = -20         );
  declareProperty ("CutMV2c10"       , m_xcutMV2c10 = -20         );
  declareProperty ("CutMV2c10_hybrid", m_xcutMV2c10_hybrid = -20  );
  declareProperty ("MethodTag"       , m_methodTag  = ""          );
  declareProperty ("Instance"        , m_instance                 );

  declareProperty ("UseBeamSpotFlag",       m_useBeamSpotFlag = false);
  declareProperty ("OverrideBeamSpotValid", m_overRideBeamSpotValid = false);

  declareMonitoredVariable("Method", m_monitorMethod);
  declareMonitoredVariable("CutCounter", m_cutCounter);
}


// -----------------------------------------------------------------------------------------------------------------


TrigBjetHypo::~TrigBjetHypo() {}


// -----------------------------------------------------------------------------------------------------------------


HLT::ErrorCode TrigBjetHypo::hltInitialize() {

  // Get message service 
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigBjetHypo" << endmsg;   

  ATH_MSG_DEBUG( "declareProperty review:" );
  ATH_MSG_DEBUG(" AcceptAll = "       << m_acceptAll );
  ATH_MSG_DEBUG( " MethodTag = "       << m_methodTag );
  ATH_MSG_DEBUG(" Instance = "        << m_instance );
  ATH_MSG_DEBUG(" UseBeamSpotFlag = " << m_useBeamSpotFlag );
  ATH_MSG_DEBUG(" JetKey = "          << m_jetKey );

  if (m_xcutMV2c20 != -20) ATH_MSG_DEBUG( " CutMV2c20 = "  << m_xcutMV2c20 );
  if (m_xcutMV2c10 != -20) ATH_MSG_DEBUG( " CutMV2c10 = "  << m_xcutMV2c10 );
  if (m_xcutMV2c10_hybrid != -20) ATH_MSG_DEBUG( " CutMV2c10_hybrid = "  << m_xcutMV2c10_hybrid );
  if (m_xcutCOMB   != -20) ATH_MSG_DEBUG( " CutXCOMB  = "  << m_xcutCOMB );
  if (m_xcutCHI2   != -20) ATH_MSG_DEBUG( " CutXCHI2  = "  << m_xcutCHI2 );
  if (m_xcutIP3D   != -20) ATH_MSG_DEBUG( " CutXIP3D  = "  << m_xcutIP3D );
  if (m_xcutIP2D   != -20) ATH_MSG_DEBUG( " CutXIP2D  = "  << m_xcutIP2D );

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBjetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  ATH_MSG_DEBUG( "Executing TrigBjetHypo" );

  // AcceptAll declare property setting 
  if (m_acceptAll) {
    ATH_MSG_DEBUG( "REGTEST: AcceptAll property is set: taking all events and applying the selection only to save the TrigPassBits" );
  } else 
    ATH_MSG_DEBUG( "REGTEST: AcceptAll property not set: applying the selection and saving the TrigPassBits" );
  
  // initialise monitoring variables 
  m_cutCounter = -1;

  // Retrieve beamspot information 
  if (m_useBeamSpotFlag) {

    IBeamCondSvc* iBeamCondSvc; 
    StatusCode sc = service("BeamCondSvc", iBeamCondSvc);
    
    if (sc.isFailure() || iBeamCondSvc == 0) {
      
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endmsg;
      
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

  // Get RoI descriptor 
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor, m_jetKey);
  
  if (stat == HLT::OK) {
    ATH_MSG_DEBUG( "Using outputTE: " << *roiDescriptor );
    //<< "RoI id " << roiDescriptor->roiId()
    //    << ", Phi = " <<  roiDescriptor->phi()
    //	    << ", Eta = " << roiDescriptor->eta() << endmsg;
  } else {
    if (msgLvl() <= MSG::WARNING) 
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endmsg;

    return HLT::NAV_ERROR;
  }

  bool result = false;

  // Define TrigPassBits for b-jets 
  //TrigPassBits *bitsEF=0;

  // Retrieve xAOD b-jet object 
  const xAOD::BTaggingContainer* trigBTaggingContainer=0;
  
  if((getFeature(outputTE, trigBTaggingContainer, "HLTBjetFex") != HLT::OK) && trigBTaggingContainer) {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to get BTaggingContainer" << endmsg;
    return HLT::OK;
  }
  if(msgLvl() <= MSG::DEBUG && trigBTaggingContainer != 0) {
    ATH_MSG_DEBUG( "Got BTaggingContainer with " << trigBTaggingContainer->size() << " BTagging object" );
  }
  if(trigBTaggingContainer && trigBTaggingContainer != 0 && trigBTaggingContainer->size() > 1) {
    if(msgLvl() <= MSG::ERROR)
      msg() << MSG::ERROR << "More than one BTagging object to analyse: this should never happen" << endmsg;
    return HLT::NAV_ERROR;
  }
  if(!trigBTaggingContainer) {
    if(msgLvl() <= MSG::ERROR)
      msg() << MSG::ERROR << "No BTagging object to analyse: this should never happen" << endmsg;
    return HLT::NAV_ERROR;
  }

  // Add TrigPassBits for EF b-jets 
  //bitsEF = HLT::makeTrigPassBits(trigBTaggingContainer);
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::BTaggingContainer>(trigBTaggingContainer);

  // to separate bad input TE and true behaviour 
  m_cutCounter=1;
  
  auto trigBTagging = trigBTaggingContainer->begin();
  auto trigBTaggingEnd = trigBTaggingContainer->end();

  // Loop over EFBjets and perform cut 
  if (m_methodTag == "MV2c20") {

    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) { 

      double x = (*trigBTagging)->auxdata<double>("MV2c20_discriminant");

      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"MV2c20 x =  " << x;
      if(x>m_xcutMV2c20) {
	//HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endmsg;
	result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endmsg;
      }
    }
  }
  else if (m_methodTag == "MV2c10") {

    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) {

      double x = (*trigBTagging)->auxdata<double>("MV2c10_discriminant");

      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "MV2c10 x =  " << x;
      if(x>m_xcutMV2c10) {
        //HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
        xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endmsg;
        result = true;
      }
      else {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endmsg;
      }
    }
  } 
  // Temporary use mv2c00 for hybrid tuning
  //  else if (m_methodTag == "MV2c10_hybrid") {
  else if (m_methodTag == "MV2c00") {
    
    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) {

      // Temporary use mv2c00 for hybrid tuning
      //      double x = (*trigBTagging)->auxdata<double>("MV2c10_hybrid_discriminant");
      double x = (*trigBTagging)->auxdata<double>("MV2c00_discriminant");

      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "MV2c10_hybrid x =  " << x;
      if(x>m_xcutMV2c10_hybrid) {
        //HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);                                                                                                                            
        xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endmsg;
        result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endmsg;
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
	//HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endmsg;
	result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endmsg;
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
	//HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
        xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endmsg;
	result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endmsg;
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
	//HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
        xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Passed " << endmsg;
	result = true;
      }
      else {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " ==> Failed " << endmsg;
      }
    }
  }

  if(result) m_cutCounter++;
  pass = result;
  
  // Print trigger decision 
  if (m_acceptAll) {
    ATH_MSG_DEBUG( "REGTEST: Trigger decision is 1" );
  } else {
    ATH_MSG_DEBUG( "REGTEST: Trigger decision is " << pass );
  }

  // Monitoring of method used to perform the cut 
  if (m_methodTag == "IP2D") m_monitorMethod = 1;
  else if (m_methodTag == "IP3D") m_monitorMethod = 2;
  else if (m_methodTag == "COMB") m_monitorMethod = 3;

  if (m_acceptAll) {
    m_cutCounter = 2;
    pass = true;
    //xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
    //return HLT::OK;
    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++)
      xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
  }

  // Print TrigPassBits to outputTE 
  //if (attachBits(outputTE, bitsEF, "EFBjets") != HLT::OK) {
  //  msg() << MSG::ERROR << "Problem attaching TrigPassBits for b-jets" << endmsg;
  //}
  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
    ATH_MSG_ERROR("Could not store TrigPassBits! ");

  ATH_MSG_DEBUG( "TrigPassBits was saved" );


  return HLT::OK;
}


// -----------------------------------------------------------------------------------------------------------------


HLT::ErrorCode TrigBjetHypo::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetHypo" << endmsg;

  return HLT::OK;
}

