/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONRESOLUTIONPLOTS_H
#define MUONHISTUTILS_MUONRESOLUTIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ResolutionPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"

namespace Muon{
  
class MuonResolutionPlots:public PlotBase {
 public:
  MuonResolutionPlots(PlotBase* pParent, const std::string& sDir, const std::string& sType="", bool doBinnedResolutions=false);
  void fill(const xAOD::TrackParticle& muontp, const xAOD::TruthParticle& truthprt, float weight=1.0);

  Trk::ResolutionPlots m_oResolutionPlots;
  TH2* Res_pT_vs_lowpT{nullptr};
  TH2* Res_pT_vs_highpT{nullptr};
  TH2* Res_pT_vs_pT{nullptr};
  TH2* Res_pT_vs_eta{nullptr};
  TH2* Res_pT_vs_phi{nullptr};

  TH2* Res_eta_vs_pT{nullptr};
  TH2* Res_phi_vs_pT{nullptr};

 private:
  std::string m_sType;
  bool m_doBinnedResolutionPlots;

};


}

#endif
