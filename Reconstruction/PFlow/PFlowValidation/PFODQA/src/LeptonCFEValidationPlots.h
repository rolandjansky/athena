/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LEPTONCFEVALIDATIONPLOTS_H
#define LEPTONCFEVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
#include "PFOHistUtils/LeptonFELinkerPlots.h"
#include "xAODEventInfo/EventInfo.h"

class LeptonCFEValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  LeptonCFEValidationPlots(PlotBase* pParent,std::string sDir, std::string LeptonContainerName);

  /** fill the histograms up */
  void fill(const xAOD::Electron& el, const xAOD::EventInfo& eventInfo);
  void fill(const xAOD::Muon& muon, const xAOD::EventInfo& eventInfo);
  void fill(const xAOD::TauJet& tau, const xAOD::EventInfo& eventInfo);
  void fill(const xAOD::Photon& phot, const xAOD::EventInfo& eventInfo);

 private:
  // Lepton plots of observables linked to FE
  PFO::LeptonFELinkerPlots m_PhotonMatchedCFEPlots;
  PFO::LeptonFELinkerPlots m_ElectronMatchedCFEPlots;
  PFO::LeptonFELinkerPlots m_MuonMatchedCFEPlots;
  PFO::LeptonFELinkerPlots m_TauJetMatchedCFEPlots;
};
#endif
