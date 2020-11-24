/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LEPTONNFEVALIDATIONPLOTS_H
#define LEPTONNFEVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "PFOHistUtils/LeptonFELinkerPlots.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"


class LeptonNFEValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  LeptonNFEValidationPlots(PlotBase* pParent, const std::string& sDir, const std::string& LeptonContainerName);

  /** fill the histograms up */
  void fill(const xAOD::Electron& el);
  void fill(const xAOD::Muon& muon);
  void fill(const xAOD::TauJet& tau);
  void fill(const xAOD::Photon& phot);

 private:
  // Lepton plots of observables linked to FE
  PFO::LeptonFELinkerPlots m_PhotonMatchedNFEPlots;
  PFO::LeptonFELinkerPlots m_ElectronMatchedNFEPlots;
  PFO::LeptonFELinkerPlots m_MuonMatchedNFEPlots;
  PFO::LeptonFELinkerPlots m_TauJetMatchedNFEPlots;
};
#endif
