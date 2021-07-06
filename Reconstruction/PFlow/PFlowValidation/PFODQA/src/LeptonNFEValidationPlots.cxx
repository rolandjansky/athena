/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LeptonNFEValidationPlots.h"

LeptonNFEValidationPlots::LeptonNFEValidationPlots(PlotBase* pParent, const std::string& sDir, const std::string& LeptonContainerName) : PlotBase(pParent, sDir),
														       m_PhotonMatchedNFEPlots(this,"/Leptonlinks/",LeptonContainerName,true,22),
														       m_ElectronMatchedNFEPlots(this,"/Leptonlinks/",LeptonContainerName,true,11),
														       m_MuonMatchedNFEPlots(this,"/Leptonlinks/",LeptonContainerName,true,13),
														       m_TauJetMatchedNFEPlots(this,"/Leptonlinks/",LeptonContainerName,true,17)
{}

void LeptonNFEValidationPlots::fill(const xAOD::Photon& phot, const xAOD::EventInfo& eventInfo){
  // photon plots
  m_PhotonMatchedNFEPlots.fill(phot,eventInfo);
}
void LeptonNFEValidationPlots::fill(const xAOD::Electron& el, const xAOD::EventInfo& eventInfo){
  //electron plots
 m_ElectronMatchedNFEPlots.fill(el,eventInfo);
}
void LeptonNFEValidationPlots::fill(const xAOD::Muon& muon, const xAOD::EventInfo& eventInfo){
  //muon plots
   m_MuonMatchedNFEPlots.fill(muon,eventInfo);
}

void LeptonNFEValidationPlots::fill(const xAOD::TauJet& tau, const xAOD::EventInfo& eventInfo){
  // taujet plots
   m_TauJetMatchedNFEPlots.fill(tau,eventInfo);
}


