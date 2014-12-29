/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_TRUTHRELATEDMUONPLOTS_H
#define MUONPHYSVALMONITORING_TRUTHRELATEDMUONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitDiffPlots.h"
#include "MuonHistUtils/MuonHitDiffSummaryPlots.h"
#include "MuonHistUtils/MuonResolutionPlots.h"
#include "TrkValHistUtils/DefParamPullPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"

class TruthRelatedMuonPlots:public PlotBase {
 public:
  TruthRelatedMuonPlots(PlotBase* pParent, std::string sDir,bool doBinnedResolutionPlots);
  ~TruthRelatedMuonPlots();
  void fill(const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu);

  // Truth related plots
  Trk::ParamPlots                 m_oMatchedPlots;
  Trk::MSHitDiffPlots             m_oMSHitDiffPlots;
  Muon::MuonHitDiffSummaryPlots   m_oMuonHitDiffSummaryPlots;
  Muon::MuonResolutionPlots       m_oMuonResolutionPlots;
  Muon::MuonResolutionPlots       m_oMuonMSResolutionPlots;
  Muon::MuonResolutionPlots       m_oMuonIDResolutionPlots;
  Trk::DefParamPullPlots          m_oDefParamPullPlots;
  Trk::DefParamPullPlots          m_oMSDefParamPullPlots;
  Trk::DefParamPullPlots          m_oIDDefParamPullPlots;
};

#endif
