// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
//#include "TrigConfHLTData/HLTTriggerElement.h"


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

    ATH_MSG_DEBUG("Initialization:");

    ATH_MSG_DEBUG("Initialization completed successfully:");
    ATH_MSG_DEBUG("AcceptAll            = " 
            << (m_acceptAll==true ? "True" : "False"));
    ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut);
    ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut);


    return HLT::OK;
}


HLT::ErrorCode TrigEFDielectronMassHypo::hltFinalize()
{
    ATH_MSG_INFO("in finalize()");

  return HLT::OK;
}

HLT::ErrorCode TrigEFDielectronMassHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
    pass = false;

    /*std::string label="";
    TrigConf::HLTTriggerElement::getLabel (outputTE->getId(), label );
    ATH_MSG_INFO(" TE Id: "<< outputTE->getId() << " TE label: " << label);*/

    std::vector<const xAOD::TrigCompositeContainer*> vms;
    const xAOD::TrigCompositeContainer *cont(0);
    if (getFeatures(outputTE, vms, "") != HLT::OK) {
        ATH_MSG_WARNING("Failed to get TrigCompositeContainer");
        return HLT::MISSING_FEATURE;
    } else {
        ATH_MSG_DEBUG("Number of TrigCompositeContainers " << vms.size());
    }
   
    cont=vms.back();
    if(!cont){
        ATH_MSG_ERROR("REGTEST: Retrieval of TrigCompositeContainer from vector failed");
        return HLT::OK;
    }
    else ATH_MSG_DEBUG("REGTEST: Number of TrigComposites " << cont->size());
    if(!checkComposites(cont)){
        ATH_MSG_ERROR("REGTEST: TrigCompositeContainer problem");
        return HLT::MISSING_FEATURE;
    }
    m_monCut = 1;
    if(m_useElectronElectron){
        if(checkMasses(cont,"elel",m_lowerMassCut,m_upperMassCut))
            pass = true;
    }
    if(m_useElectronCluster){
        if(checkMasses(cont,"elcl",m_lowerMassElectronClusterCut,m_upperMassElectronClusterCut))
            pass = true;
    }
    // set output TriggerElement true if good combination
    ATH_MSG_DEBUG("pass = " << pass);
    return HLT::OK;    
}

bool TrigEFDielectronMassHypo::checkMasses(const xAOD::TrigCompositeContainer* masses, 
    const std::string &key, const float mass_min, const float mass_max){
    
    bool status=false;
    float mass=-999.;
    m_monCut = 2;
    for(const auto comp:*masses){
        if(comp->name()==key){
            if(!comp->getDetail("mass",mass)){
                ATH_MSG_WARNING("REGTEST: Cannot retrieve mass");
                return false; 
            }          
            // apply cut on mass
            if(mass<mass_min || mass>mass_max) {
                ATH_MSG_DEBUG("Combination failed mass cut: " 
                        << mass << " not in [" << mass_min << "," << mass_max << "]");
            } else {
                // good combination found
                status = true;
                m_monCut = 3;    
                m_monMassAccepted = mass;
                ATH_MSG_DEBUG("Combination passed mass cut: " 
                        << mass_min << " < " << mass << " < " 
                        << mass_max);
                ATH_MSG_DEBUG("Good combination found! Mee=" 
                        << mass << " MeV");
            }
        }
    }
    return status;
}

//Following method for validation purposes
bool TrigEFDielectronMassHypo::checkComposites(const xAOD::TrigCompositeContainer* masses){
    float mass=-999.;
    for(const auto comp:*masses){
        if(!comp->getDetail("mass",mass)){
            ATH_MSG_ERROR("REGTEST: Cannot retrieve mass");
            return false; 
        }          
        const xAOD::Electron *tag=comp->object<xAOD::Electron>("tag");
        if(tag==nullptr){
            ATH_MSG_ERROR("REGTEST: Cannot retrieve tag");
            return false; 
        }
        ATH_MSG_DEBUG("Retrieve tag: " << tag->pt());
        if(comp->name()=="elcl"){
            const xAOD::CaloCluster *probeClus=comp->object<xAOD::CaloCluster>("probe");
            if(probeClus==nullptr){
                ATH_MSG_ERROR("REGTEST:: Cannot retrieve probe cluster");
                return false;
            }
            ATH_MSG_DEBUG("Retrieved probe: " << probeClus->pt());

            TLorentzVector hlv1 = tag->p4();
            TLorentzVector hlv2 = probeClus->p4();
            ATH_MSG_DEBUG("Mass from objects:" << (hlv1+hlv2).M()
                    << " Mass from FEX: " << mass);
        }
        if(comp->name()=="elel"){
            const xAOD::Electron *probeEl=comp->object<xAOD::Electron>("probe");
            if(probeEl==nullptr){
                ATH_MSG_ERROR("REGTEST:: Cannot retrieve probe electron");
                return false;
            }
            ATH_MSG_DEBUG("Retrieved probe: " << probeEl->pt());
            TLorentzVector hlv1 = tag->p4();
            TLorentzVector hlv2 = probeEl->p4();
            ATH_MSG_DEBUG("Mass from objects:" << (hlv1+hlv2).M()
                    << " Mass from FEX: " << mass);
        }
    }
    return true;
}
//  LocalWords:  upperMassCut
