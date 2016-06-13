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
 **   Modified:  Apr    2016 use TrigComposite
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigL2DielectronMassFex.h"
#include "TrigEgammaHypo/TrigL2DielectronMassHypo.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

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
  
    ATH_MSG_INFO("Initialization:");
  
    ATH_MSG_DEBUG("Initialization completed successfully:");
    ATH_MSG_DEBUG("AcceptAll            = " 
	<< (m_acceptAll==true ? "True" : "False"));
    ATH_MSG_DEBUG("ValidElectron        = " 
	<< (m_electronValid==true ? "True" : "False"));
    ATH_MSG_DEBUG("SameTrackAlgo        = " 
	<< (m_sameTrackAlgo==true ? "True" : "False"));
    ATH_MSG_DEBUG("OppositeCharge       = " 
	<< (m_oppositeCharge==true ? "True" : "False"));
    ATH_MSG_DEBUG("CommonVertex         = " 
	<< (m_commonVertex==true ? "True" : "False"));
    ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut);
    ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut);
  
  return HLT::OK;
}


HLT::ErrorCode TrigL2DielectronMassHypo::hltFinalize()
{
    ATH_MSG_INFO("in finalize()");
    return HLT::OK;
}

HLT::ErrorCode TrigL2DielectronMassHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  pass=false;
  m_monCut = 0;

  std::vector<const xAOD::TrigCompositeContainer*> vms;
  if (getFeatures(outputTE, vms) != HLT::OK) {
      ATH_MSG_WARNING("Failed to get TrigCompositeContainer");
    return HLT::MISSING_FEATURE;
  } else {
      ATH_MSG_DEBUG("Number of TrigCompositeContainers " << vms.size());
  }
  m_monCut = 1;

  const xAOD::TrigCompositeContainer *cont=vms.back();
  if(!cont){
      ATH_MSG_ERROR("REGTEST: Retrieval of TrigCompositeContainer from vector failed");
      return HLT::OK;
  }
  else ATH_MSG_DEBUG("REGTEST: Number of TrigComposites " << cont->size()); 

  float mass=-999.;
  for(const auto &comp:*cont){
      if(comp->name()!="L2Dielectron"){
          ATH_MSG_WARNING("REGTEST: Not L2Dielectron TrigComposite");
          continue;
      }
      if(!comp->getDetail("Mee",mass)){
          ATH_MSG_ERROR("REGTEST: Cannot retrieve Mee");
          return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      }
      m_monCut = 2;
      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {
          ATH_MSG_DEBUG("Combination failed mass cut: " 
                  << mass << " not in [" << m_lowerMassCut << "," << m_upperMassCut << "]");
      } else {
          // good combination found
          pass = true;
          m_monCut = 3;    
          m_monMassAccepted = mass;
          ATH_MSG_DEBUG("Combination passed mass cut: " 
                  << m_lowerMassCut << " < " << mass << " < " 
                  << m_upperMassCut);
          ATH_MSG_DEBUG("Good combination found! Mee=" 
                  << mass << " MeV");
      }
  } // End of loop over TrigComposites in L2DielectronInfo   

  ATH_MSG_DEBUG("pass = " << pass);
  return HLT::OK;    
}
