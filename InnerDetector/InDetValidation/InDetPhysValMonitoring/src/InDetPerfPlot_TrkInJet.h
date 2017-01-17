/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPERFPLOT_TRKINJET_H
#define INDETPERFPLOT_TRKINJET_H

#include "InDetPlotBase.h"
#include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "TProfile.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODJet/Jet.h"

class InDetPerfPlot_TrkInJet: public InDetPlotBase {
public:
  InDetPerfPlot_TrkInJet(InDetPlotBase* pParent, std::string sDir);
  bool fill(const xAOD::TrackParticle& trk, const xAOD::Jet& jet);
  bool fillCounter(const xAOD::Jet& jet);

  bool BookEffReco(const xAOD::TruthParticle& truth, const xAOD::Jet& jet);
  bool BookEffTruth(const xAOD::TruthParticle& truth, const xAOD::Jet& jet);
  bool fillEff(const xAOD::Jet& jet);

  void clearCounters();
  void clearEffCounters();


  TH1* m_jetPt;
  TH1* m_recPt;
  TH1* m_nTrack;
  TH1* m_sumPt;
  TH1* m_fracPt;

  TProfile* n_vs_jetDR_BLayerHits;
  TProfile* n_vs_jetDR_BLayerOutliers;
  TProfile* n_vs_jetDR_BLayerSharedHits;
  TProfile* n_vs_jetDR_BLayerSplitHits;
  TProfile* n_vs_jetDR_PixelHits;
  TProfile* n_vs_jetDR_PixelHoles;
  TProfile* n_vs_jetDR_PixelOutliers;
  TProfile* n_vs_jetDR_PixelContribLayers;
  TProfile* n_vs_jetDR_PixelSharedHits;
  TProfile* n_vs_jetDR_PixelSplitHits;
  TProfile* n_vs_jetDR_PixelGangedHits;
  TProfile* n_vs_jetDR_SCTHits;
  TProfile* n_vs_jetDR_SCTHoles;
  TProfile* n_vs_jetDR_SCTOutliers;
  TProfile* n_vs_jetDR_SCTDoubleHoles;
  TProfile* n_vs_jetDR_SCTSharedHits;
  TProfile* n_vs_jetDR_TRTHits;
  TProfile* n_vs_jetDR_TRTHighThresholdHits;
  TProfile* n_vs_jetDR_TRTOutliers;
  TProfile* n_vs_jetDR_TRTHighThresholdOutliers;

  TProfile* n_vs_jetDR;
  TProfile* sumPt_vs_jetDR;
  TProfile* fracPt_vs_jetDR;
  TProfile* n_vs_jetPt;
  TProfile* sumPt_vs_jetPt;
  TProfile* fracPt_vs_jetPt;

  TProfile* n_vs_jetDR_reco;
  TProfile* sumPt_vs_jetDR_reco;
  TProfile* fracPt_vs_jetDR_reco;
  TProfile* n_vs_jetPt_reco;
  TProfile* sumPt_vs_jetPt_reco;
  TProfile* fracPt_vs_jetPt_reco;
  TProfile* n_vs_jetDR_truth;
  TProfile* sumPt_vs_jetDR_truth;
  TProfile* fracPt_vs_jetDR_truth;
  TProfile* n_vs_jetPt_truth;
  TProfile* sumPt_vs_jetPt_truth;
  TProfile* fracPt_vs_jetPt_truth;

  TH1* n_vs_jetDR_eff;
  TH1* sumPt_vs_jetDR_eff;
  TH1* n_vs_jetPt_eff;
  TH1* sumPt_vs_jetPt_eff;
  ///@}
private:
  void initializePlots();
  void finalizePlots();

  int m_dRNBins, m_jetPtNBins;
  float m_dRMin, m_dRMax, m_jetPtMin, m_jetPtMax;
  float m_dRWidth;
  float m_etaMin, m_etaMax;

  std::vector<int>   m_tracksVsDr;
  std::vector<float> m_sumPtVsDr;
  std::vector<int>   m_tracksVsDr_reco;
  std::vector<float> m_sumPtVsDr_reco;
  std::vector<int>   m_tracksVsDr_truth;
  std::vector<float> m_sumPtVsDr_truth;
};

#endif
