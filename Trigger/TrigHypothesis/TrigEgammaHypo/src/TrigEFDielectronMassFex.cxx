// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigEFDielectronMassFex.h
 **
 **   Description: - Hypothesis algorithm: searc for electron pairs with 
 **                invariant mass in some interval; intended for Z->ee
 **                - Modified from TrigL2DielectronMassFex by R.Goncalo
 **
 **   Author: T. Hrynova  <thrynova@mail.cern.ch>
 **
 **   Created:   Nov 9 2009 
 **
 **   Author(xAOD): Ryan Mackenzie White
 **   Modified: April 2014
 **************************************************************************/ 
#include <sstream>

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigEgammaHypo/TrigEFDielectronMassFex.h"

#include "TrigSteeringEvent/TrigOperationalInfo.h"

#include "TLorentzVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <math.h>

TrigEFDielectronMassFex::TrigEFDielectronMassFex(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator) {

  // Read cuts
  declareProperty("useElectronElectron", m_useElectronElectron=true, 
		  "Use electron-electron pair to calculate invariant mass");
  declareProperty("useElectronCluster", m_useElectronCluster=false, 
		  "Use electron-cluster pair to calculate invariant mass");
  declareProperty("AcceptAll", m_acceptAll);
  declareProperty("LowerMassCut", m_lowerMassCut=50000.0);
  declareProperty("UpperMassCut", m_upperMassCut=130000.0);
  
  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("cut", m_monCut);
}

TrigEFDielectronMassFex::~TrigEFDielectronMassFex()
{ }

HLT::ErrorCode TrigEFDielectronMassFex::hltInitialize()
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


HLT::ErrorCode TrigEFDielectronMassFex::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigEFDielectronMassFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {
  pass = false;

  m_monMassAccepted = -1.;
  m_monCut=0;
  // sanity checks
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigEFDielectronMassFex::acceptInputs" << endreq;

  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got diferent than 2 number of input TEs: " << 
      inputTE.size() << " job badly configured" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	    << endreq;
    pass = true;
    return HLT::OK;
  } else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	    << endreq;
  }

  // this are 2 TEs which we eventually will combine
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];

  // for debugging purpose look into RoIDescriptors
  if ( msgLvl() <= MSG::DEBUG ){
    const TrigRoiDescriptor* roiDescriptor1 = 0;
    const TrigRoiDescriptor* roiDescriptor2 = 0;
    if ( getFeature(te1, roiDescriptor1) != HLT::OK || getFeature(te2, roiDescriptor2) != HLT::OK ) {
      if ( msgLvl() <= MSG::WARNING) {
	msg() <<  MSG::WARNING << "No RoIDescriptors for this Trigger Elements! " << endreq;
      }  
    } else {
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2 << endreq;
    }
  }
  
  // retrieve TrigElectronContainers from this TE
  const xAOD::ElectronContainer* electronCont(0);
  const xAOD::CaloClusterContainer* clusterCont(0);
  if (m_useElectronElectron) {
    if ( getFeature(te1, electronCont) != HLT::OK || electronCont == 0 ||
	 getFeature(te2, electronCont) != HLT::OK || electronCont == 0) {
      // Not an error condition as it could happen for e+etcut chain
      if ( msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Failed to get egammaContainer" << endreq;
      }
      return HLT::MISSING_FEATURE;
    }
  }
  if (m_useElectronCluster) {
    if ( getFeature(te1, electronCont) != HLT::OK || electronCont == 0 ||
	 getFeature(te2, clusterCont) != HLT::OK || clusterCont == 0) {
      // Not an error condition as it could happen for e+etcut chain
      if ( msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG 
	      << "Failed to get egammaContainer/clusterContainer" << endreq;
      }
      return HLT::MISSING_FEATURE;
    }
  }

  pass=true;

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "pass = " << pass << endreq;
  }
  return HLT::OK;    
}  

HLT::ErrorCode TrigEFDielectronMassFex::hltExecute(HLT::TEConstVec& inputTE, 
						   HLT::TriggerElement* outputTE){

  m_monMassAccepted = -1.;
  m_monCut=0;
  m_massElectronElectron.clear();
  m_massElectronCluster.clear();

  unsigned int i;
  if (m_useElectronElectron) {
    TrigOperationalInfo* masses_ee = new TrigOperationalInfo();
    processElectronElectron(inputTE, m_massElectronElectron);
    for (i=0; i<m_massElectronElectron.size(); ++i) {
      std::ostringstream os;
      os << "mass_ee" << i;
      masses_ee->set(os.str(), m_massElectronElectron[i]);
    }
    attachFeature(outputTE, masses_ee, "MassesElectronElectron");
  }
  if (m_useElectronCluster) {
    TrigOperationalInfo* masses_ec = new TrigOperationalInfo();
    processElectronCluster(inputTE, m_massElectronCluster);
    for (i=0; i<m_massElectronCluster.size(); ++i) {
      std::ostringstream os;
      os << "mass_ec" << i;
      masses_ec->set(os.str(), m_massElectronCluster[i]);
    }
    attachFeature(outputTE, masses_ec, "MassesElectronCluster");
  }

  return HLT::OK;
}

void TrigEFDielectronMassFex::processElectronElectron(HLT::TEConstVec& inputTE, 
						      std::vector<float>& masses) {
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];
  const xAOD::ElectronContainer* cont1(0);
  const xAOD::ElectronContainer* cont2(0);
  if (getFeature(te1, cont1) != HLT::OK || getFeature(te2, cont2) != HLT::OK ||
      cont1 == 0 || cont2 == 0) {
    // Not an error condition as it could happen for e+etcut chain
    if ( msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG 
	    << "Failed to get egammaContainers" << endreq;
    }
    return;
  }

  float mass(0.0);
  for (const auto& p1 : *cont1){
      for (const auto& p2 : *cont2){
      m_monCut = 1;
      // selection is done here
      //
      // debug dump (both electrons have tracks now)

      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "New combination:" << endreq; 
	msg() << MSG::DEBUG << "1st egammaElectron " 
	      << "  pt="    << p1->pt()  
	      << "; eta="   << p1->eta() 
	      << "; phi="   << p1->phi() 
	      << endreq;
	msg() << MSG::DEBUG << "2nd egammaElectron " 
	      << "  pt="    << p2->pt()  
	      << "; eta="   << p2->eta()                 
	      << "; phi="   << p2->phi()
	      << endreq;
      }

      // evaluate mass
      
      TLorentzVector hlv1 = p1->p4();
      TLorentzVector hlv2 = p2->p4();
      mass = (hlv1+hlv2).M();

//       if (msgLvl() <= MSG::DEBUG) {
// 	TLorentzVector v1, v2;
// 	const float m_e = 0.511;

// 	float pt1 = hlv1.e()*sin(hlv1.theta());
// 	float pt2 = hlv2.e()*sin(hlv2.theta());
// 	v1.SetPtEtaPhiM(pt1, hlv1.eta(), hlv1.phi(), m_e);
// 	v2.SetPtEtaPhiM(pt2, hlv2.eta(), hlv2.phi(), m_e);
//       }

      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination failed mass cut: " 
		<< mass << " not in [" << m_lowerMassCut << "," 
		<< m_upperMassCut << "]" << endreq;
	}
	continue;
      } else {
	// good combination found
	m_monCut = 2;
	if (masses.size() == 0) m_monMassAccepted = mass;
	masses.push_back(mass);
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination passed mass cut: " 
		<< m_lowerMassCut << " < " << mass << " < " 
		<< m_upperMassCut << endreq;
	}
      }   
    } // electrons2 container loop end
  } // electrons1 container loop end
}

void TrigEFDielectronMassFex::processElectronCluster(HLT::TEConstVec& inputTE, 
						     std::vector<float>& masses) {
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];
  const xAOD::ElectronContainer* cont1(0);
  const xAOD::CaloClusterContainer* cont2(0);
  if (getFeature(te1, cont1) != HLT::OK || getFeature(te2, cont2) != HLT::OK ||
      cont1 == 0 || cont2 == 0) {
    // Not an error condition as it could happen for e+etcut chain
    if ( msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG 
	    << "Failed to get egammaContainer & CaloClusterContainer" 
	    << endreq;
    }
    return;
  }

  float mass(0.0);
  for (const auto& p1 : *cont1){
      for (const auto& p2 : *cont2){
      m_monCut = 1;
      // selection is done here
      //
      // debug dump (both electrons have tracks now)

      //Need to fix for xAOD!!!!!!!!!!!!
      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "New combination:" << endreq; 
	msg() << MSG::DEBUG << "1st egammaElectron " 
	      << "  pt="    << p1->pt()  
	      << "; eta="   << p1->eta() 
	      << "; phi="   << p1->phi() 
	      << endreq;
	msg() << MSG::DEBUG << "2nd CaloCluster:"
	      << "  pt="    << p2->pt()  
	      << "; eta="   << p2->eta()                 
	      << "; phi="   << p2->phi()
	      << endreq;
      }

      // evaluate mass
      
      TLorentzVector hlv1 = p1->p4();
      TLorentzVector hlv2 = p2->p4();
      mass = (hlv1+hlv2).M();

      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination failed mass cut: " 
		<< mass << " not in [" << m_lowerMassCut << "," 
		<< m_upperMassCut << "]" << endreq;
	}
	continue;
      } else {
	// good combination found
	m_monCut = 2;
	if (masses.size() == 0) m_monMassAccepted = mass;
	masses.push_back(mass);
	if (msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination passed mass cut: " 
		<< m_lowerMassCut << " < " << mass << " < " 
		<< m_upperMassCut << endreq;
	}
      }  
    } // electrons2 container loop end
  } // electrons1 container loop end
}

