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

void LeptonNFEValidationPlots::fill(const xAOD::Photon& phot){
  // photon plots
  m_PhotonMatchedNFEPlots.fill(phot);
}
void LeptonNFEValidationPlots::fill(const xAOD::Electron& el){
  //electron plots
 m_ElectronMatchedNFEPlots.fill(el);
}
void LeptonNFEValidationPlots::fill(const xAOD::Muon& muon){
  //muon plots
   m_MuonMatchedNFEPlots.fill(muon);
}

void LeptonNFEValidationPlots::fill(const xAOD::TauJet& tau){
  // taujet plots
   m_TauJetMatchedNFEPlots.fill(tau);
}


