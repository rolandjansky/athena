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

class MomentumDiffPlots:public PlotBase {
 public:
  MomentumDiffPlots(PlotBase* pParent, std::string sDir);
  ~MomentumDiffPlots();

  TH1* dp_CB_truthIP;
  TH1* dp_ME_truthIP;
  TH1* dp_ME_truthIP_MS_truthMS;

  TH1* dp_CB_truthIP_Tail;
  TH1* dp_ME_truthIP_Tail;
  TH1* dp_ME_truthIP_MS_truthMS_Tail;

  TH1* dp_eloss;
  
  void fill(const xAOD::Muon& muon, const xAOD::TrackParticle* muontpIP, const xAOD::TruthParticle& truthprt);
  void initializePlots();
};
}

#endif
