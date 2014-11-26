/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EgammaValidationPlots.h"

EgammaValidationPlots::EgammaValidationPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
										  // Reco only information
										  m_oElectronAllPlots(this, "Electron/All/", "Reco Electron"),
										  m_oElectronTruthPlots(this, "Electron/Truth/", "Truth Electron"),
										  m_oPhotonAllPlots(this, "Photon/All/", "Reco Photon"),
										  m_oPhotonTruthPlots(this, "Photon/Truth/", "Truth Photon")
{}	

void EgammaValidationPlots::fill(const xAOD::Electron& electron) {
  m_oElectronAllPlots.fill(electron);
}

void EgammaValidationPlots::fill(const xAOD::Photon& photon){
  m_oPhotonAllPlots.fill(photon);
}
