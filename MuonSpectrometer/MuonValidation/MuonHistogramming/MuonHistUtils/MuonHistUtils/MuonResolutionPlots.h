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
  MuonResolutionPlots(PlotBase* pParent, std::string sDir,bool doBinnedResolutionPlots);
  ~MuonResolutionPlots();
  void fill(const xAOD::TrackParticle& muontp, const xAOD::TruthParticle& truthprt);

  Trk::ResolutionPlots            m_oResolutionPlots;
      
  std::vector<Trk::ResolutionPlots*> m_oResolutionPlots_ptBins;
  std::vector<Trk::ResolutionPlots*> m_oResolutionPlots_etaBins;
  std::vector<Trk::ResolutionPlots*> m_oResolutionPlots_phiBins;
  std::vector<float> resolution_ptBins;
  std::vector<float> resolution_etaBins;
  std::vector<float> resolution_phiBins;

  TProfile *Res_pT_vs_pT;
  TProfile *Res_pT_vs_eta;
  TProfile *Res_pT_vs_phi;

  int findBin(float x,std::vector<float> bins);

};
}

#endif
