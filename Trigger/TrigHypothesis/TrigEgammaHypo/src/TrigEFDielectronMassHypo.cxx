// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigEFDielectronMassHypo.h
 **
 **   Description: - Hypothesis algorithm: searc for electron pairs with 
 **                invariant mass in some interval; intended for Z->ee
 **                - Modified from TrigL2DielectronMassHypo by R. Goncalo
 **
 **   Author: T. Hrynova  <thrynova@mail.cern.ch>
 **
 **   Created:   Nov 13 2009
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigEFDielectronMassFex.h"

#include "TrigEgammaHypo/TrigEFDielectronMassHypo.h"

#include "TrigSteeringEvent/TrigOperationalInfo.h"


class ISvcLocator;

TrigEFDielectronMassHypo::TrigEFDielectronMassHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll);
  declareProperty("useElectronElectron", m_useElectronElectron=true, 
		  "Use electron-electron pair to calculate invariant mass");
  declareProperty("useElectronCluster", m_useElectronCluster=false, 
		  "Use electron-cluster pair to calculate invariant mass");
  declareProperty("LowerMassCut", m_lowerMassCut=50000.0);
  declareProperty("UpperMassCut", m_upperMassCut=130000.0);
  declareProperty("LowerMassElectronClusterCut", 
		  m_lowerMassElectronClusterCut=50000.0, 
		  "Lower mass cut for electron-cluster pair");
  declareProperty("UpperMassElectronClusterCut", 
		  m_upperMassElectronClusterCut=130000.0, 
		  "Upper mass cut for electron-cluster pair");

  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("cut", m_monCut);

}

TrigEFDielectronMassHypo::~TrigEFDielectronMassHypo()
{ }

HLT::ErrorCode TrigEFDielectronMassHypo::hltInitialize()
{
  
  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
  }

  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
    msg() << MSG::DEBUG << "AcceptAll            = " 
	  << (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
  }
  
  return HLT::OK;
}


HLT::ErrorCode TrigEFDielectronMassHypo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}

HLT::ErrorCode TrigEFDielectronMassHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  pass = false;
  const TrigOperationalInfo* x(0);

  if (m_useElectronElectron) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Check electron-electron pair masses" << endreq;
    }
    if (getFeature(outputTE, x, "MassesElectronElectron") != HLT::OK) {
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Error while getting features " << endreq;
      }
    } else {
      if (checkAllMasses(x, m_lowerMassCut, m_upperMassCut)) {
	pass = true;
      }
    }
  }
  if (m_useElectronCluster) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Check electron-cluster pair masses" << endreq;
    }
    if (getFeature(outputTE, x, "MassesElectronCluster") != HLT::OK) {
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Error while getting features " << endreq;
      }
    } else {
      if (checkAllMasses(x, m_lowerMassElectronClusterCut, 
			 m_upperMassElectronClusterCut)) {
	pass = true;
      }
    }
  }

  // set output TriggerElement true if good combination
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endreq;
  return HLT::OK;    
}

bool TrigEFDielectronMassHypo::checkAllMasses(const TrigOperationalInfo* x, 
					      float mass_min, float mass_max) {
  bool status=false;
  if (x) {
    std::pair<std::vector<std::string>, std::vector<float> > mp = x->infos();
    //std::vector<std::string>& names = mp.first;
    std::vector<float>& values = mp.second;
    for (unsigned int i=0; i<values.size(); ++i) {
      float mass = values[i];
      if (mass < mass_min || mass > mass_max) {
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination failed mass cut: " 
		<< mass << " not in [" << mass_min << "," 
		<< mass_max << "]" << endreq;
	}
      } else {
	status = true;
	m_monCut = 2;
	m_monMassAccepted = mass;
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination passed mass cut: " 
		<< mass_min << " < " << mass << " < " 
		<< mass_max << endreq;	     
	  msg() << MSG::DEBUG << "Good combination found! Mee=" 
		<< mass << " MeV" << endreq;
	}
      }
    }
  }
  return status;
}

//  LocalWords:  upperMassCut
