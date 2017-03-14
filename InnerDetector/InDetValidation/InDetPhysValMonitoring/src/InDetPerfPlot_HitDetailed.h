/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPERFPLOT_HITDETAILED_H
#define INDETPERFPLOT_HITDETAILED_H

#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticleFwd.h"

class TH1;
class TProfile;

class InDetPerfPlot_HitDetailed: public InDetPlotBase {
public:
  InDetPerfPlot_HitDetailed(InDetPlotBase* pParent, std::string sDir);
  void fill(const xAOD::TrackParticle& trk);
  // 1D histograms
  TH1* nBLayerHits;
  TH1* nBLayerOutliers;
  TH1* nBLayerSharedHits;
  TH1* nBLayerSplitHits;
  TH1* nPixelHits;
  TH1* nPixelHoles;
  TH1* nPixelOutliers;
  TH1* nPixelContribLayers;
  TH1* nPixelSharedHits;
  TH1* nPixelSplitHits;
  TH1* nPixelGangedHits;
  TH1* nPixelGangedHitsFlaggedFakes;
  TH1* nSCTHits;
  TH1* nSCTHoles;
  TH1* nSCTOutliers;
  TH1* nSCTDoubleHoles;
  TH1* nSCTSharedHits;
  TH1* nTRTHits;
  TH1* nTRTHighThresholdHits;
  TH1* nTRTOutliers;
  TH1* nTRTHighThresholdOutliers;
  // eta profiles
  TProfile* n_vs_eta_BLayerHits;
  TProfile* n_vs_eta_BLayerOutliers;
  TProfile* n_vs_eta_BLayerSharedHits;
  TProfile* n_vs_eta_BLayerSplitHits;
  TProfile* n_vs_eta_PixelHits;
  TProfile* n_vs_eta_PixelHoles;
  TProfile* n_vs_eta_PixelOutliers;
  TProfile* n_vs_eta_PixelContribLayers;
  TProfile* n_vs_eta_PixelSharedHits;
  TProfile* n_vs_eta_PixelSplitHits;
  TProfile* n_vs_eta_PixelGangedHits;
  TProfile* n_vs_eta_PixelGangedHitsFlaggedFakes;
  TProfile* n_vs_eta_SCTHits;
  TProfile* n_vs_eta_SCTHoles;
  TProfile* n_vs_eta_SCTOutliers;
  TProfile* n_vs_eta_SCTDoubleHoles;
  TProfile* n_vs_eta_SCTSharedHits;
  TProfile* n_vs_eta_TRTHits;
  TProfile* n_vs_eta_TRTHighThresholdHits;
  TProfile* n_vs_eta_TRTOutliers;
  TProfile* n_vs_eta_TRTHighThresholdOutliers;
private:
  void initializePlots();
};

#endif
