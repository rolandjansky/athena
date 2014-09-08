// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2DielectronMassHypo.h
 **
 **   Description: - Hypothesis algorithm: searc for electron pairs with 
 **                invariant mass in some interval; intended for Z->ee
 **                - Modified from TrigL2DimuonMassHypo by C.Schiavi
 **
 **   Author: R.Goncalo  <r.goncalo@rhul.ac.uk>
 **
 **   Created:   Aug 13 2006
 **   Modified:  Jan 19 2007 Valeria Perez Reale Adapted to New Steering  
 **   Modified:  Apr 28 2007 Tomasz Bold major changes to run with new steering
 **   Modified:  May    2011 major changes to use TrigOperatioanlInfo
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigL2DielectronMassFex.h"
#include "TrigEgammaHypo/TrigL2DielectronMassHypo.h"

#include "TrigSteeringEvent/TrigOperationalInfo.h"

class ISvcLocator;

TrigL2DielectronMassHypo::TrigL2DielectronMassHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll);
  declareProperty("SameTrackAlgo",m_sameTrackAlgo=true);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("CommonVertex", m_commonVertex=true);
  declareProperty("ValidElectron",m_electronValid=true);
//   declareProperty("LowerMassCut", m_lowerMassCut=50000.0);
//   declareProperty("UpperMassCut", m_upperMassCut=130000.0);
  declareProperty("LowerMassCut", m_lowerMassCut=1000.0);
  declareProperty("UpperMassCut", m_upperMassCut=6000.0);

  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("cut", m_monCut);

}

TrigL2DielectronMassHypo::~TrigL2DielectronMassHypo()
{ }

HLT::ErrorCode TrigL2DielectronMassHypo::hltInitialize()
{
  
  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
  }

  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
    msg() << MSG::DEBUG << "AcceptAll            = " 
	<< (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "ValidElectron        = " 
	<< (m_electronValid==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "SameTrackAlgo        = " 
	<< (m_sameTrackAlgo==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "OppositeCharge       = " 
	<< (m_oppositeCharge==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "CommonVertex         = " 
	<< (m_commonVertex==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
  }
  
  return HLT::OK;
}


HLT::ErrorCode TrigL2DielectronMassHypo::hltFinalize()
{
 if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}

HLT::ErrorCode TrigL2DielectronMassHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  pass=false;
  m_monCut = 0;

  // retrieve TrigOperationalInfo //
  std::vector<const TrigOperationalInfo*> vms;
  if (getFeatures(outputTE, vms) != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigOperationalInfo" << endreq;
    }
    return HLT::MISSING_FEATURE;
  } else {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Number of TrigOperationalInfo: " << vms.size() << endreq;
    }
  }
  m_monCut = 1;


  std::vector<const TrigOperationalInfo*>::const_iterator pm;
  for (pm=vms.begin(); pm!=vms.end(); ++pm) {
    // check if this TrigOperationalInfo has L2DielectronInfo
    if ( !((*pm)->defined("mass0")==1) ) continue;
    unsigned int  Ninfos = (*pm)->infos().first.size();
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Number of Info belonging to  TrigOperationalInfo: " << Ninfos << endreq;
    }
    for(unsigned int iInfo=0; iInfo<Ninfos; ++iInfo ) {
      char key[64];
      std::sprintf(key, "mass%d", iInfo);
      if ( !(*pm)->defined(std::string(key)) ) break;

      float mass = (*pm)->get(std::string(key));

      m_monCut = 2;
      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination failed mass cut: " 
		<< mass << " not in [" << m_lowerMassCut << "," << m_upperMassCut << "]" << endreq;
	}
      } else {
	// good combination found
	pass = true;
	m_monCut = 3;    
	m_monMassAccepted = mass;
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination passed mass cut: " 
		<< m_lowerMassCut << " < " << mass << " < " 
		<< m_upperMassCut << endreq;	     
	  msg() << MSG::DEBUG << "Good combination found! Mee=" 
		<< mass << " MeV" << endreq;
	}
      }    
    }// End of loop over infos in L2DielectronInfo
  }// End of loop over TrigOperationalInfo


  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endreq;
  return HLT::OK;    
}
