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
  std::vector<const xAOD::TrigCompositeContainer*> vms;
  const xAOD::TrigCompositeContainer *cont(0);
  if (m_useElectronElectron) {
      if (getFeatures(outputTE, vms, "MassesElectronElectron") != HLT::OK) {
          ATH_MSG_WARNING("Failed to get TrigCompositeContainer");
          return HLT::MISSING_FEATURE;
      } else {
          ATH_MSG_DEBUG("Number of TrigCompositeContainers " << vms.size());
      }
      m_monCut = 1;

      cont=vms.back();
      if(!cont){
          ATH_MSG_ERROR("REGTEST: Retrieval of TrigCompositeContainer from vector failed");
          return HLT::OK;
      }
      else ATH_MSG_DEBUG("REGTEST: Number of TrigComposites " << cont->size());
      if (checkAllMasses(cont, m_lowerMassCut, m_upperMassCut)) {
	pass = true;
      }
  }
  if (m_useElectronCluster) {
      if (getFeatures(outputTE, vms, "MassesElectronCluster") != HLT::OK) {
          ATH_MSG_WARNING("Failed to get TrigCompositeContainer");
          return HLT::MISSING_FEATURE;
      } else {
          ATH_MSG_DEBUG("Number of TrigCompositeContainers " << vms.size());
      }
      m_monCut = 1;
      cont=vms.back();
      if(!cont){
          ATH_MSG_ERROR("REGTEST: Retrieval of TrigCompositeContainer from vector failed");
          return HLT::OK;
      }
      else ATH_MSG_DEBUG("REGTEST: Number of TrigComposites " << cont->size());
      if (checkAllMasses(cont, m_lowerMassCut, m_upperMassCut)) {
	pass = true;
      }
  }
 

  // set output TriggerElement true if good combination
  ATH_MSG_DEBUG("pass = " << pass);
  return HLT::OK;    
}

bool TrigEFDielectronMassHypo::checkAllMasses(const xAOD::TrigCompositeContainer* masses, 
					      float mass_min, float mass_max) {
  bool status=false;
  float mass=-999.;
  for(const auto &comp:*masses){
      if(comp->name()!="EFDielectron"){
          ATH_MSG_WARNING("REGTEST: Not EFDielectron TrigComposite");
          continue;
      }
      if(!comp->getDetail("Mee",mass)){
          ATH_MSG_ERROR("REGTEST: Cannot retrieve Mee");
          return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      }
      m_monCut = 2;
      // apply cut on mass
      if(mass<mass_min || mass>mass_max) {
          ATH_MSG_DEBUG("Combination failed mass cut: " 
                  << mass << " not in [" << m_lowerMassCut << "," << m_upperMassCut << "]");
      } else {
          // good combination found
          status = true;
          m_monCut = 3;    
          m_monMassAccepted = mass;
          ATH_MSG_DEBUG("Combination passed mass cut: " 
                  << m_lowerMassCut << " < " << mass << " < " 
                  << m_upperMassCut);
          ATH_MSG_DEBUG("Good combination found! Mee=" 
                  << mass << " MeV");
      }
  } // End of loop over TrigComposites in EFDielectronInfo   
  
  return status;
}

//  LocalWords:  upperMassCut
