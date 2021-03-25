/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LeptonCFEValidationPlots.h"

LeptonCFEValidationPlots::LeptonCFEValidationPlots(PlotBase* pParent, std::string sDir, std::string LeptonContainerName) : PlotBase(pParent, sDir),
															   m_PhotonMatchedCFEPlots(this,"/Leptonlinks/",LeptonContainerName,false,22),
															   m_ElectronMatchedCFEPlots(this,"/Leptonlinks/",LeptonContainerName,false,11),
															   m_MuonMatchedCFEPlots(this,"/Leptonlinks/",LeptonContainerName,false,13),
															   m_TauJetMatchedCFEPlots(this,"/Leptonlinks/",LeptonContainerName,false,17)
{}

void LeptonCFEValidationPlots::fill(const xAOD::Muon& muon, const xAOD::EventInfo& eventInfo){
  //muon plots
  m_MuonMatchedCFEPlots.fill(muon,eventInfo);
}
void LeptonCFEValidationPlots::fill(const xAOD::Electron& el, const xAOD::EventInfo& eventInfo){
  //electron plots
  m_ElectronMatchedCFEPlots.fill(el,eventInfo);
}
void LeptonCFEValidationPlots::fill(const xAOD::Photon& phot, const xAOD::EventInfo& eventInfo){
  // photon plots
  m_PhotonMatchedCFEPlots.fill(phot,eventInfo);
}
void LeptonCFEValidationPlots::fill(const xAOD::TauJet& tau, const xAOD::EventInfo& eventInfo){
  // taujet plots
  m_TauJetMatchedCFEPlots.fill(tau,eventInfo);
}


