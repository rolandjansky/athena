/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MOMENTUMTRUTHPULLPLOTS_H
#define MUONHISTUTILS_MOMENTUMTRUTHPULLPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"

namespace Muon{

class MomentumTruthPullPlots:public PlotBase {
 public:
  MomentumTruthPullPlots(PlotBase* pParent, const std::string& sDir, std::string sType);
  ~MomentumTruthPullPlots();

  std::string m_sType;
  
  TH1* dp_CB_truthIP{nullptr};
  TH1* dp_ME_truthIP{nullptr};
  TH1* dp_ME_truthIP_MS_truthMS{nullptr};
  TH1* dp_eloss{nullptr};

  TH2* dp_CB_truthIP_vs_pt{nullptr};
  TH2* dp_ME_truthIP_vs_pt{nullptr};
  TH2* dp_ME_truthIP_MS_truthMS_vs_pt{nullptr};
  TH2* dp_eloss_vs_pt{nullptr};

  TH2* dp_CB_truthIP_vs_eta{nullptr};
  TH2* dp_ME_truthIP_vs_eta{nullptr};
  TH2* dp_ME_truthIP_MS_truthMS_vs_eta{nullptr};
  TH2* dp_eloss_vs_eta{nullptr};

  
  void fill(const xAOD::Muon& muon, const xAOD::TrackParticle* muontpIP, const xAOD::TruthParticle& truthprt, float weight=1.0);
  void initializePlots();
};

 
}//namespace Muon

#endif
