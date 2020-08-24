// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
 **  
 **   Updated to use TrigComposite Objects for release 21.
 **   @ R. White <ryan.white@cern.ch> October 2016 
 **   Algorithm reconstructs Tag and Probe pairs
 **   using well-identified electrons from the first TE as tag
 **   the probe can be either electron or cluster from the second TE.
 **   Entries for each TrigComposite object:
 **   name
 **   mass
 **   tag (elementlink)
 **   probe (elementlink)
 **************************************************************************/ 
#include <sstream>

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigEgammaHypo/TrigEFDielectronMassFex.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TLorentzVector.h"
//#include "TrigConfHLTData/HLTTriggerElement.h"
#include <math.h>


TrigEFDielectronMassFex::TrigEFDielectronMassFex(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator) {

  // Read cuts
  declareProperty("AcceptAll", m_acceptAll);
  declareProperty("LowerMassCut", m_lowerMassCut=50000.0);
  declareProperty("UpperMassCut", m_upperMassCut=130000.0);
  declareProperty("OutputKey", m_key="TrigEFDielectronMassFex");
  
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

    const xAOD::ElectronContainer* cont1(0);
    const xAOD::ElectronContainer* cont2(0);
    const xAOD::CaloClusterContainer* clusters(0);

    ElementLinkVector< xAOD::ElectronContainer > links1;
    ElementLinkVector< xAOD::ElectronContainer > links2;
    ElementLinkVector< xAOD::CaloClusterContainer > cllinks;
    if (getFeature(te1, cont1) != HLT::OK ||
            getFeature(te2, cont2) != HLT::OK ||
            getFeature(te2, clusters) != HLT::OK || 
            cont1 == 0 || 
            cont2 == 0 ||
            clusters == 0) {
        ATH_MSG_ERROR("Failed to get Electron or Cluster containers");
        return HLT::MISSING_FEATURE;
    }
    
    // Useful for debugging
    /*std::string label1="";
    TrigConf::HLTTriggerElement::getLabel (inputTE[0]->getId(), label1 );
    std::string label2="";
    TrigConf::HLTTriggerElement::getLabel (inputTE[1]->getId(), label2 );
    ATH_MSG_INFO(" TE Id: "<< inputTE[0]->getId() << " TE label: " 
            << label1 << " electrons (tags): " << cont1->size());
    ATH_MSG_INFO(" TE Id: "<< inputTE[1]->getId() << " TE label: " 
            << label2 << " electrons (probes) " << cont2->size() << " cluster (probes) " << clusters->size());*/


    if (getFeaturesLinks<xAOD::ElectronContainer,xAOD::ElectronContainer>(te1, links1,"") != HLT::OK 
            || getFeaturesLinks<xAOD::ElectronContainer,xAOD::ElectronContainer>(te2, links2,"") != HLT::OK
            || getFeaturesLinks<xAOD::CaloClusterContainer,xAOD::CaloClusterContainer>(te2, cllinks,"") != HLT::OK){ 
        // Not an error condition as it could happen for e+etcut chain
        ATH_MSG_ERROR("Failed to get ElementLinkContainers");
        return HLT::MISSING_FEATURE;
    }
    const xAOD::TrigPassBits* bits(0);
    HLT::ErrorCode status = getFeature(te1, bits, "passbits");
    if (status != HLT::OK) {
        ATH_MSG_WARNING(" Failed to get TrigPassBits ");
        return HLT::MISSING_FEATURE;
    }

    pass=true;

    ATH_MSG_DEBUG("pass = " << pass);
    return HLT::OK;    
}  

HLT::ErrorCode TrigEFDielectronMassFex::hltExecute(HLT::TEConstVec& inputTE, 
						   HLT::TriggerElement* outputTE){

  m_monMassAccepted = -1.;
  m_monCut=0;
  ATH_MSG_DEBUG("hltExecute()");
  xAOD::TrigCompositeContainer *cont=new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer cont_aux;
  cont->setStore(&cont_aux);
  process(inputTE, cont);
  attachFeature(outputTE, cont, m_key);

  return HLT::OK;
}

void TrigEFDielectronMassFex::process(HLT::TEConstVec& inputTE, xAOD::TrigCompositeContainer *objs){ 

    ATH_MSG_DEBUG("process");

    const HLT::TriggerElement* te1 = inputTE[0];
    const HLT::TriggerElement* te2 = inputTE[1];
    const xAOD::ElectronContainer* cont1(0);
    const xAOD::ElectronContainer* cont2(0);
    const xAOD::CaloClusterContainer* clusters(0);

    ElementLinkVector< xAOD::ElectronContainer > links1;
    ElementLinkVector< xAOD::ElectronContainer > links2;
    ElementLinkVector< xAOD::CaloClusterContainer > cllinks;
    if (getFeature(te1, cont1) != HLT::OK ||
            getFeature(te2, cont2) != HLT::OK ||
            getFeature(te2, clusters) != HLT::OK || 
            cont1 == 0 || 
            cont2 == 0 ||
            clusters == 0) {
        ATH_MSG_WARNING("Failed to get Electron or Cluster containers");
        return;
    }

    if (getFeaturesLinks<xAOD::ElectronContainer,xAOD::ElectronContainer>(te1, links1,"") != HLT::OK 
            || getFeaturesLinks<xAOD::ElectronContainer,xAOD::ElectronContainer>(te2, links2,"") != HLT::OK
            || getFeaturesLinks<xAOD::CaloClusterContainer,xAOD::CaloClusterContainer>(te2, cllinks,"") != HLT::OK){ 
        // Not an error condition as it could happen for e+etcut chain
        ATH_MSG_WARNING("Failed to get ElementLinkContainers");
        return;
    }

    const xAOD::TrigPassBits* bits(0);
    HLT::ErrorCode status = getFeature(te1, bits, "passbits");
    if (status != HLT::OK) {
        ATH_MSG_WARNING(" Failed to get TrigPassBits ");
        return; 
    }
    m_monCut = 1;
    float mass(0.0);
    for (const auto& link1 : links1){
        // Ensure we use good tag
        const xAOD::Electron* p1 = *link1;
        ATH_MSG_DEBUG("Tag Electron " 
                << "  pt="    << p1->pt()  
                << "; eta="   << p1->eta() 
                << "; phi="   << p1->phi());
        if(!bits->isPassing(p1,cont1)){
            ATH_MSG_DEBUG("Tag Electron found not passing Hypo object");
            continue;
        }
        for (const auto& link2 : links2){
            const xAOD::Electron* p2 = *link2;
            m_monCut=2;
            // selection is done here
            ATH_MSG_DEBUG("Electron Electron combination:");
            ATH_MSG_DEBUG("Probe Electron " 
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
                m_monCut = 3;
                m_monMassAccepted = mass;
                xAOD::TrigComposite *comp=new xAOD::TrigComposite();
                objs->push_back(comp);
                comp->setName("elel");
                comp->setDetail("mass",mass);
                comp->setObjectLink("tag",link1);
                comp->setObjectLink("probe",link2);
                ATH_MSG_DEBUG("Combination passed mass cut: " 
                        << m_lowerMassCut << " < " << mass << " < " 
                        << m_upperMassCut);

            }   
        } // electrons2 container loop end
        // Now combine with cluster
        for (const auto& cllink : cllinks){
            const xAOD::CaloCluster *p2 = *cllink;
            m_monCut = 4;

            ATH_MSG_DEBUG("Electron Cluster combination:");
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
                m_monCut = 5;
                m_monMassAccepted = mass;
                xAOD::TrigComposite *comp=new xAOD::TrigComposite();
                objs->push_back(comp);
                comp->setName("elcl");
                comp->setDetail("mass",mass);
                comp->setObjectLink("tag",link1);
                comp->setObjectLink("probe",cllink);
                ATH_MSG_DEBUG("Combination passed mass cut: " 
                        << m_lowerMassCut << " < " << mass << " < " 
                        << m_upperMassCut);
            }
        } // cluster container loop end
    } // electrons1 container loop end
}

