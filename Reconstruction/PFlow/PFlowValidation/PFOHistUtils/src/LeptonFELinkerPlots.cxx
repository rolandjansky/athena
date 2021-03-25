/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/LeptonFELinkerPlots.h"
#include <iostream>
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "AthLinks/ElementLink.h"

namespace PFO {
  LeptonFELinkerPlots::LeptonFELinkerPlots(PlotBase* pParent, std::string sDir, std::string LeptonContainerName,const bool& doNeutralFE,const int& PID): 
    PlotBase(pParent, sDir),
    m_LeptonContainerName(LeptonContainerName),
    m_doNeutralFE(doNeutralFE),
    m_LeptonID(PID)
    {
      m_electron_NMatchedCFE=nullptr;
      m_electron_NMatchedNFE=nullptr;

      m_muon_NMatchedCFE=nullptr;
      m_muon_NMatchedNFE=nullptr;

      m_tau_NMatchedCFE=nullptr;
      m_tau_NMatchedNFE=nullptr;

      m_photon_NMatchedCFE=nullptr;
      m_photon_NMatchedNFE=nullptr;
    }

  void LeptonFELinkerPlots::initializePlots(){
    if(m_doNeutralFE)
      {
	if(m_LeptonID==11)
	  m_electron_NMatchedNFE=Book1D("_electron_NMatchedNFE",m_LeptonContainerName+"_elecron_NMatchedNFE",20,0,20);
	else if (m_LeptonID==13)
	  m_muon_NMatchedNFE=Book1D("_muon_NMatchedNFE",m_LeptonContainerName+"_muon_NMatchedNFE",20,0,20);
	else if (m_LeptonID==17)
	  m_tau_NMatchedNFE=Book1D("_tau_NMatchedNFE",m_LeptonContainerName+"_tau_NMatchedNFE",20,0,20);
	else 
	  m_photon_NMatchedNFE=Book1D("_photon_NMatchedNFE",m_LeptonContainerName+"_photon_NMatchedNFE",20,0,20);
      }
    else{
      if(m_LeptonID==11)
	m_electron_NMatchedCFE=Book1D("_electron_NMatchedCFE",m_LeptonContainerName+"_elecron_NMatchedCFE",20,0,20);
      else if (m_LeptonID==13)
	m_muon_NMatchedCFE=Book1D("_muon_NMatchedCFE",m_LeptonContainerName+"_muon_NMatchedCFE",20,0,20);
      else if(m_LeptonID==17)
	m_tau_NMatchedCFE=Book1D("_tau_NMatchedCFE",m_LeptonContainerName+"_tau_NMatchedCFE",20,0,20);
      else
	m_photon_NMatchedCFE=Book1D("_photon_NMatchedCFE",m_LeptonContainerName+"_photon_NMatchedCFE",20,0,20);
    };
  }
  
  void LeptonFELinkerPlots::fill(const xAOD::Photon& phot, const xAOD::EventInfo& eventInfo){
    int nMatched_FE=0;
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::FlowElementContainer> > >acc_FE_phot_link_NFE("neutralFELinks");
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::FlowElementContainer> > >acc_FE_phot_link_CFE("neutralFELinks");
    if(acc_FE_phot_link_NFE.isAvailable(phot) and m_doNeutralFE){
      nMatched_FE=(acc_FE_phot_link_NFE(phot)).size();
      m_photon_NMatchedNFE->Fill(nMatched_FE,eventInfo.beamSpotWeight());
    }
    else if (acc_FE_phot_link_CFE.isAvailable(phot) and !m_doNeutralFE){
      nMatched_FE=(acc_FE_phot_link_CFE(phot)).size();
      m_photon_NMatchedCFE->Fill(nMatched_FE,eventInfo.beamSpotWeight());
    }
  }
  void LeptonFELinkerPlots::fill(const xAOD::Electron& el, const xAOD::EventInfo& eventInfo){
    int nMatched_FE=0;
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::FlowElementContainer> > >acc_FE_el_link_NFE("neutralFELinks");
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::FlowElementContainer> > >acc_FE_el_link_CFE("chargedFELinks");
    
    if(acc_FE_el_link_NFE.isAvailable(el) and m_doNeutralFE){
      nMatched_FE=(acc_FE_el_link_NFE(el)).size();
      m_electron_NMatchedNFE->Fill(nMatched_FE,eventInfo.beamSpotWeight());
    }
    else if(acc_FE_el_link_CFE.isAvailable(el) and !m_doNeutralFE){
      nMatched_FE=(acc_FE_el_link_CFE(el)).size();
      m_electron_NMatchedCFE->Fill(nMatched_FE,eventInfo.beamSpotWeight());
    }
  }
 
  void LeptonFELinkerPlots::fill(const xAOD::Muon& mu, const xAOD::EventInfo& eventInfo){
    int nMatched_FE=0;
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::FlowElementContainer> > >acc_FE_mu_link_NFE("neutralFELinks");
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::FlowElementContainer> > >acc_FE_mu_link_CFE("chargedFELinks");
    if(acc_FE_mu_link_NFE.isAvailable(mu) and m_doNeutralFE){
      nMatched_FE=(acc_FE_mu_link_NFE(mu)).size();
      m_muon_NMatchedNFE->Fill(nMatched_FE,eventInfo.beamSpotWeight());    
    }
    else if(acc_FE_mu_link_CFE.isAvailable(mu) and !m_doNeutralFE){
      nMatched_FE=(acc_FE_mu_link_CFE(mu)).size();
      m_muon_NMatchedCFE->Fill(nMatched_FE,eventInfo.beamSpotWeight());    
    }
  }
 
  void LeptonFELinkerPlots::fill(const xAOD::TauJet& tau, const xAOD::EventInfo& eventInfo){
    int nMatched_FE=0;
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::FlowElementContainer> > >acc_FE_tau_link_NFE("neutralFELinks");
    SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::FlowElementContainer> > >acc_FE_tau_link_CFE("chargedFELinks");
    if(acc_FE_tau_link_NFE.isAvailable(tau) and m_doNeutralFE){

      nMatched_FE=(acc_FE_tau_link_NFE(tau)).size();
      m_tau_NMatchedNFE->Fill(nMatched_FE,eventInfo.beamSpotWeight());
    }
    else if(acc_FE_tau_link_NFE.isAvailable(tau) and !m_doNeutralFE){
      nMatched_FE=(acc_FE_tau_link_CFE(tau)).size();
      m_tau_NMatchedCFE->Fill(nMatched_FE,eventInfo.beamSpotWeight());
    }
 
  }


  
} // end of PFO namespace
