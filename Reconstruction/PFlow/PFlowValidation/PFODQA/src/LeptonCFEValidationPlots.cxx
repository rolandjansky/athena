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

void LeptonCFEValidationPlots::fill(const xAOD::Muon& muon){
  //muon plots
  m_MuonMatchedCFEPlots.fill(muon);
}
void LeptonCFEValidationPlots::fill(const xAOD::Electron& el){
  //electron plots
  m_ElectronMatchedCFEPlots.fill(el);
}
void LeptonCFEValidationPlots::fill(const xAOD::Photon& phot){
  // photon plots
  m_PhotonMatchedCFEPlots.fill(phot);
}
void LeptonCFEValidationPlots::fill(const xAOD::TauJet& tau){
  // taujet plots
  m_TauJetMatchedCFEPlots.fill(tau);
}


