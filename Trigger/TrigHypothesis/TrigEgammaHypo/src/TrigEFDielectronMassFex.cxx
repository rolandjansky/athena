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
  
    ATH_MSG_INFO("Initialization:");
  
    ATH_MSG_DEBUG("Initialization completed successfully:");
    ATH_MSG_DEBUG("AcceptAll            = " 
	  << (m_acceptAll==true ? "True" : "False"));
    ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut);
    ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut);
  
  return HLT::OK;
}


HLT::ErrorCode TrigEFDielectronMassFex::hltFinalize()
{
  
    ATH_MSG_INFO("in finalize()");

  return HLT::OK;
}


HLT::ErrorCode TrigEFDielectronMassFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {
  pass = false;

  m_monMassAccepted = -1.;
  m_monCut=0;
  // sanity checks
  ATH_MSG_DEBUG("Running TrigEFDielectronMassFex::acceptInputs");

  if ( inputTE.size() != 2 ) {
    ATH_MSG_ERROR("Got diferent than 2 number of input TEs: " << 
      inputTE.size() << " job badly configured");
    return HLT::BAD_JOB_SETUP;
  }

  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
      ATH_MSG_DEBUG("AcceptAll property is set: taking all events");
      pass = true;
      return HLT::OK;
  } else {
      ATH_MSG_DEBUG("AcceptAll property not set: applying selection");
  }

  // this are 2 TEs which we eventually will combine
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];

  // for debugging purpose look into RoIDescriptors
  /*if ( msgLvl() <= MSG::DEBUG ){
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
  }*/
  
  // retrieve TrigElectronContainers from this TE
  const xAOD::ElectronContainer* electronCont(0);
  const xAOD::CaloClusterContainer* clusterCont(0);
  if (m_useElectronElectron) {
    if ( getFeature(te1, electronCont) != HLT::OK || electronCont == 0 ||
	 getFeature(te2, electronCont) != HLT::OK || electronCont == 0) {
      // Not an error condition as it could happen for e+etcut chain
	ATH_MSG_DEBUG("Failed to get egammaContainer");
      return HLT::MISSING_FEATURE;
    }
  }
  if (m_useElectronCluster) {
    if ( getFeature(te1, electronCont) != HLT::OK || electronCont == 0 ||
	 getFeature(te2, clusterCont) != HLT::OK || clusterCont == 0) {
      // Not an error condition as it could happen for e+etcut chain
        ATH_MSG_DEBUG("Failed to get egammaContainer/clusterContainer");
      return HLT::MISSING_FEATURE;
    }
  }

  pass=true;

  ATH_MSG_DEBUG("pass = " << pass);
  return HLT::OK;    
}  

HLT::ErrorCode TrigEFDielectronMassFex::hltExecute(HLT::TEConstVec& inputTE, 
						   HLT::TriggerElement* outputTE){

  m_monMassAccepted = -1.;
  m_monCut=0;
  m_massElectronElectron.clear();
  m_massElectronCluster.clear();
  m_cont=new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer contaux;
  m_cont->setStore(&contaux);

  if (m_useElectronElectron) {
    processElectronElectron(inputTE, m_massElectronElectron);
    for (const auto &mass:m_massElectronElectron) {
        xAOD::TrigComposite *comp=new xAOD::TrigComposite();;
        m_cont->push_back(comp);
        comp->setName("EFDielectron");
        comp->setDetail("Mee",mass);
    }
    attachFeature(outputTE, m_cont, "MassesElectronElectron");
  }
  if (m_useElectronCluster) {
    processElectronCluster(inputTE, m_massElectronCluster);
    for (const auto &mass:m_massElectronCluster){
        xAOD::TrigComposite *comp=new xAOD::TrigComposite();;
        m_cont->push_back(comp);
        comp->setName("EFDielectron");
        comp->setDetail("Mee",mass);
    }
    attachFeature(outputTE, m_cont, "MassesElectronCluster");
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

	ATH_MSG_DEBUG("New combination:");
	ATH_MSG_DEBUG("1st egammaElectron " 
	      << "  pt="    << p1->pt()  
	      << "; eta="   << p1->eta() 
	      << "; phi="   << p1->phi());
	ATH_MSG_DEBUG("2nd egammaElectron " 
	      << "  pt="    << p2->pt()  
	      << "; eta="   << p2->eta()                 
	      << "; phi="   << p2->phi());

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
        ATH_MSG_DEBUG("Combination passed mass cut: " 
                << m_lowerMassCut << " < " << mass << " < " 
                << m_upperMassCut);
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
      ATH_MSG_DEBUG( "Failed to get egammaContainer & CaloClusterContainer"); 
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
	ATH_MSG_DEBUG("New combination:");
	ATH_MSG_DEBUG("1st egammaElectron " 
	      << "  pt="    << p1->pt()  
	      << "; eta="   << p1->eta() 
	      << "; phi="   << p1->phi()); 
	ATH_MSG_DEBUG("2nd CaloCluster:"
	      << "  pt="    << p2->pt()  
	      << "; eta="   << p2->eta()                 
	      << "; phi="   << p2->phi());

      // evaluate mass
      
      TLorentzVector hlv1 = p1->p4();
      TLorentzVector hlv2 = p2->p4();
      mass = (hlv1+hlv2).M();

      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {
	  ATH_MSG_DEBUG("Combination failed mass cut: " 
		<< mass << " not in [" << m_lowerMassCut << "," 
		<< m_upperMassCut << "]");
	continue;
      } else {
	// good combination found
	m_monCut = 2;
	if (masses.size() == 0) m_monMassAccepted = mass;
	masses.push_back(mass);
        ATH_MSG_DEBUG("Combination passed mass cut: " 
            << m_lowerMassCut << " < " << mass << " < " 
            << m_upperMassCut);
      }  
    } // electrons2 container loop end
  } // electrons1 container loop end
}

