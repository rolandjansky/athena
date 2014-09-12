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

#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"

#include "TrigSteeringEvent/TrigPassBits.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetHypo::TrigBjetHypo(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator),
  m_cutCounter(0)
{
  declareProperty ("AcceptAll", m_acceptAll);
  declareProperty ("CutXIP2D",  m_xcutIP2D = -20);
  declareProperty ("CutXIP3D",  m_xcutIP3D = -20);
  declareProperty ("CutXCOMB",  m_xcutCOMB = -20);
  declareProperty ("CutXCHI2",  m_xcutCHI2 = -20);
  declareProperty ("MethodTag", m_methodTag = "");
  declareProperty ("Instance",  m_instance);

  declareProperty ("UseBeamSpotFlag", m_useBeamSpotFlag = false);

  declareMonitoredVariable("Method", m_monitorMethod);
  declareMonitoredVariable("CutCounter", m_cutCounter);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetHypo::~TrigBjetHypo() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetHypo::hltInitialize() {

  //* Get message service *//
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigBjetHypo" << endreq;   

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " AcceptAll = " << m_acceptAll << endreq; 
    msg() << MSG::DEBUG << " MethodTag = " << m_methodTag << endreq; 
    msg() << MSG::DEBUG << " Instance = "  << m_instance  << endreq; 
    msg() << MSG::DEBUG << " UseBeamSpotFlag = " << m_useBeamSpotFlag << endreq; 

    if (m_xcutCOMB != -20) msg() << MSG::DEBUG << " CutXCOMB = "  << m_xcutCOMB  << endreq; 
    if (m_xcutCHI2 != -20) msg() << MSG::DEBUG << " CutXCHI2 = "  << m_xcutCHI2  << endreq; 
    if (m_xcutIP3D != -20) msg() << MSG::DEBUG << " CutXIP3D = "  << m_xcutIP3D  << endreq; 
    if (m_xcutIP2D != -20) msg() << MSG::DEBUG << " CutXIP2D = "  << m_xcutIP2D  << endreq; 
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigBjetHypo" << endreq;

  //* AcceptAll declare property setting *//
  if (m_acceptAll) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: AcceptAll property is set: taking all events and applying the selection only for saving the TrigPassBits" << endreq;
  } else 
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: AcceptAll property not set: applying the selection and saving the TrigPassBits" << endreq;
  
  //* initialise monitoring variables *//
  m_cutCounter = -1;
  
  //* Retrieve beamspot information *//
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

  //* Get RoI descriptor *//
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor, "");
  
  if (stat == HLT::OK) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Using outputTE: " 
	    << "RoI id " << roiDescriptor->roiId()
	    << ", Phi = " <<  roiDescriptor->phi()
	    << ", Eta = " << roiDescriptor->eta() << endreq;
    }
  } else {
    if (msgLvl() <= MSG::WARNING) 
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endreq;

    return HLT::NAV_ERROR;
  }

  bool result = false;

  //* Define TrigPassBits for b-jets *//
  TrigPassBits *bitsL2=0, *bitsEF=0;

  if (m_instance == "L2") {

    //* Get vector of L2Bjet *//
    const TrigL2BjetContainer* trigL2BjetColl = 0;
    if (getFeature(outputTE, trigL2BjetColl, "L2BjetFex") != HLT::OK) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "Failed to get TrigL2Bjet collection" << endreq;
      return HLT::OK;
    }

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Got collection with " << trigL2BjetColl->size() << " TrigL2Bjet" << endreq;

    if (trigL2BjetColl->size() > 1) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "More than one L2Bjet to analyse: this should never happen" << endreq;
      return HLT::NAV_ERROR;
    }

    if (trigL2BjetColl->size() == 0) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "No L2Bjet to analyse: this should never happen" << endreq;
      return HLT::NAV_ERROR;
    }

    //* Add TrigPassBits for LVL2 b-jets *//
    bitsL2 = HLT::makeTrigPassBits(trigL2BjetColl);

    //* to separate bad input TE and true behaviour *//
    m_cutCounter=1;
    
    TrigL2BjetContainer::const_iterator pBjet = trigL2BjetColl->begin();
    TrigL2BjetContainer::const_iterator lastBjet = trigL2BjetColl->end();

    //* Loop over L2Bjets and perform cut *//
    if (m_methodTag == "COMB") {
      for ( ; pBjet != lastBjet; pBjet++) { 
	if ((*pBjet)->xComb() > m_xcutCOMB) {
	  HLT::markPassing(bitsL2, (*pBjet), trigL2BjetColl); 
	  result = true;
	}
      }
    } else if (m_methodTag == "CHI2") {
      for ( ; pBjet != lastBjet; pBjet++) { 
	if ((*pBjet)->xCHI2() > m_xcutCHI2) {
	  HLT::markPassing(bitsL2, (*pBjet), trigL2BjetColl); 
	  result = true;
	}
      }
    } else if (m_methodTag == "IP3D") {
      for ( ; pBjet != lastBjet; pBjet++) { 
	if ((*pBjet)->xIP3D() > m_xcutIP3D) {
	  HLT::markPassing(bitsL2, (*pBjet), trigL2BjetColl); 
	  result = true;
	}
      }
    } else if (m_methodTag == "IP2D") {
      for ( ; pBjet != lastBjet; pBjet++) { 
	if ((*pBjet)->xIP2D() > m_xcutIP2D) {
	  HLT::markPassing(bitsL2, (*pBjet), trigL2BjetColl); 
	  result = true;
	}
      }
    }
  }

  else if (m_instance == "EF")  { 

    const TrigEFBjetContainer* trigEFBjetColl = 0;    
    if (getFeature(outputTE, trigEFBjetColl, "EFBjetFex") != HLT::OK) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "Failed to get TrigEFBjet collection" << endreq;
      return HLT::OK;
    }
    
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Got collection with " << trigEFBjetColl->size() 
	    << " TrigEFBjet" << endreq;
    }
    
    if (trigEFBjetColl->size() > 1) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "More than one EFBjet to analyse: this should never happen" << endreq;
      return HLT::NAV_ERROR;
    }

    if (trigEFBjetColl->size() == 0) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "No EFBjet to analyse: this should never happen" << endreq;
      return HLT::NAV_ERROR;
    }

    //* Add TrigPassBits for EF b-jets *//
    bitsEF = HLT::makeTrigPassBits(trigEFBjetColl);

    //* to separate bad input TE and true behaviour *//
    m_cutCounter=1;
    
    TrigEFBjetContainer::const_iterator pBjet = trigEFBjetColl->begin();
    TrigEFBjetContainer::const_iterator lastBjet = trigEFBjetColl->end();

    //* Loop over EFBjets and perform cut */
    if (m_methodTag == "COMB") {
      for ( ; pBjet != lastBjet; pBjet++) { 
	if ((*pBjet)->xComb() > m_xcutCOMB) {
	  HLT::markPassing(bitsEF, (*pBjet), trigEFBjetColl);
	  result = true;
	}
      }
    } else if (m_methodTag == "CHI2") {
      for ( ; pBjet != lastBjet; pBjet++) { 
	if ((*pBjet)->xCHI2() > m_xcutCHI2) {
	  HLT::markPassing(bitsEF, (*pBjet), trigEFBjetColl);
	  result = true;
	}
      }
    } else if (m_methodTag == "IP3D") {
      for ( ; pBjet != lastBjet; pBjet++) { 
	if ((*pBjet)->xIP3D() > m_xcutIP3D) {
	  HLT::markPassing(bitsEF, (*pBjet), trigEFBjetColl);
	  result = true;
	}
      }
    } else if (m_methodTag == "IP2D") {
      for ( ; pBjet != lastBjet; pBjet++) { 
	if ((*pBjet)->xIP2D() > m_xcutIP2D) {
	  HLT::markPassing(bitsEF, (*pBjet), trigEFBjetColl);
	  result = true;
	}
      }
    }
  }

  if(result) m_cutCounter++;
  pass = result;
  
  //* Print trigger decision *//
  if (m_acceptAll) {
    if(msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: Trigger decision is 1" << endreq;
  } else {
    if(msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "REGTEST: Trigger decision is " << pass << endreq;
  }

  //* Monitoring of method used to perform the cut *//
  if (m_methodTag == "CHI2")      m_monitorMethod = 0;
  else if (m_methodTag == "IP2D") m_monitorMethod = 1;
  else if (m_methodTag == "IP3D") m_monitorMethod = 2;
  else if (m_methodTag == "COMB") m_monitorMethod = 3;

  //* Print TrigPassBits to outputTE *//
  if (m_instance == "L2") {
    if (attachBits(outputTE, bitsL2, "L2Bjets") != HLT::OK) {
      msg() << MSG::ERROR << "Problem attaching TrigPassBits for b-jets" << endreq;
    }
  } else if (m_instance == "EF") {
    if (attachBits(outputTE, bitsEF, "EFBjets") != HLT::OK) {
      msg() << MSG::ERROR << "Problem attaching TrigPassBits for b-jets" << endreq;
    }
  }
 
  if (m_acceptAll) {
    m_cutCounter = 2;
    pass = true;
    return HLT::OK;
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetHypo::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetHypo" << endreq;

  return HLT::OK;
}

