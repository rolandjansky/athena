/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_HitDetailed.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"



InDetPerfPlot_HitDetailed::InDetPerfPlot_HitDetailed(InDetPlotBase* pParent, std::string sDir) : InDetPlotBase(pParent,
                                                                                                               sDir),
  // 1D hit hisrograms
  nBLayerHits{},
  nBLayerOutliers{},
  nBLayerSharedHits{},
  nBLayerSplitHits{},
  nPixelHits{},
  nPixelHoles{},
  nPixelOutliers{},
  nPixelContribLayers{},
  nPixelSharedHits{},
  nPixelSplitHits{},
  nPixelGangedHits{},
  nPixelGangedHitsFlaggedFakes{},
  nSCTHits{},
  nSCTHoles{},
  nSCTOutliers{},
  nSCTDoubleHoles{},
  nSCTSharedHits{},
  nTRTHits{},
  nTRTHighThresholdHits{},
  nTRTOutliers{},
  nTRTHighThresholdOutliers{},
  // eta profiles
  n_vs_eta_BLayerHits{},
  n_vs_eta_BLayerOutliers{},
  n_vs_eta_BLayerSharedHits{},
  n_vs_eta_BLayerSplitHits{},
  n_vs_eta_PixelHits{},
  n_vs_eta_PixelHoles{},
  n_vs_eta_PixelOutliers{},
  n_vs_eta_PixelContribLayers{},
  n_vs_eta_PixelSharedHits{},
  n_vs_eta_PixelSplitHits{},
  n_vs_eta_PixelGangedHits{},
  n_vs_eta_PixelGangedHitsFlaggedFakes{},
  n_vs_eta_SCTHits{},
  n_vs_eta_SCTHoles{},
  n_vs_eta_SCTOutliers{},
  n_vs_eta_SCTDoubleHoles{},
  n_vs_eta_SCTSharedHits{},
  n_vs_eta_TRTHits{},
  n_vs_eta_TRTHighThresholdHits{},
  n_vs_eta_TRTOutliers{},
  n_vs_eta_TRTHighThresholdOutliers{} {
}

void
InDetPerfPlot_HitDetailed::initializePlots() {
  // 1D histograms
  book(nBLayerHits, "HitContent_NBlayerHits");
  book(nPixelHits, "HitContent_NPixelHits");
  book(nPixelHoles, "HitContent_NPixelHoles");
  book(nSCTHits, "HitContent_NSCTHits");
  book(nSCTHoles, "HitContent_NSCTHoles");
  book(nTRTHits, "HitContent_NTRTHits");
  book(nTRTHighThresholdHits, "HitContent_NTRTHighThresholdHits");
  // eta profiles
  book(n_vs_eta_BLayerHits, "HitContent_vs_eta_NBlayerHits");
  book(n_vs_eta_PixelHits, "HitContent_vs_eta_NPixelHits");
  book(n_vs_eta_PixelHoles, "HitContent_vs_eta_NPixelHoles");
  book(n_vs_eta_SCTHits, "HitContent_vs_eta_NSCTHits");
  book(n_vs_eta_SCTHoles, "HitContent_vs_eta_NSCTHoles");
  book(n_vs_eta_TRTHits, "HitContent_vs_eta_NTRTHits");
  book(n_vs_eta_TRTHighThresholdHits, "HitContent_vs_eta_NTRTHighThresholdHits");
  if (m_iDetailLevel >= 100) {
    // 1D histograms
    book(nBLayerOutliers, "HitContent_NBlayerOutliers");
    book(nBLayerSharedHits, "HitContent_NBlayerSharedHits");
    book(nBLayerSplitHits, "HitContent_NBLayerSplitHits");
    book(nPixelOutliers, "HitContent_NPixelOutliers");
    book(nPixelContribLayers, "HitContent_NPixelContribLayers");
    book(nPixelSharedHits, "HitContent_NPixelSharedHits");
    book(nPixelSplitHits, "HitContent_NPixelSplitHits");
    book(nPixelGangedHits, "HitContent_NPixelGangedHits");
    book(nPixelGangedHitsFlaggedFakes, "HitContent_NPixelGangedHitsFlaggedFakes");
    book(nSCTOutliers, "HitContent_NSCTOutliers");
    book(nSCTDoubleHoles, "HitContent_NSCTDoubleHoles");
    book(nSCTSharedHits, "HitContent_NSCTSharedHits");
    book(nTRTOutliers, "HitContent_NTRTOutliers");
    book(nTRTHighThresholdOutliers, "HitContent_NTRTHighThresholdOutliers");
    // eta profiles
    book(n_vs_eta_BLayerOutliers, "HitContent_vs_eta_NBlayerOutliers");
    book(n_vs_eta_BLayerSharedHits, "HitContent_vs_eta_NBlayerSharedHits");
    book(n_vs_eta_BLayerSplitHits, "HitContent_vs_eta_NBLayerSplitHits");
    book(n_vs_eta_PixelOutliers, "HitContent_vs_eta_NPixelOutliers");
    book(n_vs_eta_PixelContribLayers, "HitContent_vs_eta_NPixelContribLayers");
    book(n_vs_eta_PixelSharedHits, "HitContent_vs_eta_NPixelSharedHits");
    book(n_vs_eta_PixelSplitHits, "HitContent_vs_eta_NPixelSplitHits");
    book(n_vs_eta_PixelGangedHits, "HitContent_vs_eta_NPixelGangedHits");
    book(n_vs_eta_PixelGangedHitsFlaggedFakes, "HitContent_vs_eta_NPixelGangedHitsFlaggedFakes");
    book(n_vs_eta_SCTOutliers, "HitContent_vs_eta_NSCTOutliers");
    book(n_vs_eta_SCTDoubleHoles, "HitContent_vs_eta_NSCTDoubleHoles");
    book(n_vs_eta_SCTSharedHits, "HitContent_vs_eta_NSCTSharedHits");
    book(n_vs_eta_TRTOutliers, "HitContent_vs_eta_NTRTOutliers");
    book(n_vs_eta_TRTHighThresholdOutliers, "HitContent_vs_eta_NTRTHighThresholdOutliers");
  }
}

void
InDetPerfPlot_HitDetailed::fill(const xAOD::TrackParticle& trk) {
  float eta = trk.eta();

  uint8_t iBLayerHits(0), iPixHits(0), iSctHits(0), iTrtHits(0);
  uint8_t iPixHoles(0), iSCTHoles(0), iTrtHTHits(0);

  if (trk.summaryValue(iBLayerHits, xAOD::numberOfInnermostPixelLayerHits)) {
    fillHisto(nBLayerHits, iBLayerHits);
    fillHisto(n_vs_eta_BLayerHits, eta, iBLayerHits);
  }
  if (trk.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    fillHisto(nPixelHits, iPixHits);
    fillHisto(n_vs_eta_PixelHits, eta, iPixHits);
  }
  if (trk.summaryValue(iSctHits, xAOD::numberOfSCTHits)) {
    fillHisto(nSCTHits, iSctHits);
    fillHisto(n_vs_eta_SCTHits, eta, iSctHits);
  }
  if (trk.summaryValue(iTrtHits, xAOD::numberOfTRTHits)) {
    fillHisto(nTRTHits, iTrtHits);
    fillHisto(n_vs_eta_TRTHits, eta, iTrtHits);
  }
  if (trk.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    fillHisto(nPixelHoles, iPixHoles);
    fillHisto(n_vs_eta_PixelHoles, eta, iPixHoles);
  }
  if (trk.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    fillHisto(nSCTHoles, iSCTHoles);
    fillHisto(n_vs_eta_SCTHoles, eta, iSCTHoles);
  }
  if (trk.summaryValue(iTrtHTHits, xAOD::numberOfTRTHighThresholdHits)) {
    fillHisto(nTRTHighThresholdHits, iTrtHTHits);
    fillHisto(n_vs_eta_TRTHighThresholdHits, eta, iTrtHTHits);
  }

  // expert plots
  if (m_iDetailLevel >= 100) {
    uint8_t iBLayerOutliers(0), iBLayerShared(0), iBLayerSplit(0);
    uint8_t iPixelOutliers(0), iPixelContribLayers(0), iPixelShared(0), iPixelSplit(0), iPixelGanged(0), iPixelGangedFF(
      0);
    uint8_t iSCTOutliers(0), iSCTDoubleHoles(0), iSCTShared(0);
    uint8_t iTRTOutliers(0), iTRTHTOutliers(0);
    if (trk.summaryValue(iBLayerOutliers, xAOD::numberOfInnermostPixelLayerOutliers)) {
      fillHisto(nBLayerOutliers, iBLayerOutliers);
      fillHisto(n_vs_eta_BLayerOutliers, eta, iBLayerOutliers);
    }
    if (trk.summaryValue(iBLayerShared, xAOD::numberOfInnermostPixelLayerSharedHits)) {
      fillHisto(nBLayerSharedHits, iBLayerShared);
      fillHisto(n_vs_eta_BLayerSharedHits, eta, iBLayerShared);
    }
    if (trk.summaryValue(iBLayerSplit, xAOD::numberOfInnermostPixelLayerSplitHits)) {
      fillHisto(nBLayerSplitHits, iBLayerSplit);
      fillHisto(n_vs_eta_BLayerSplitHits, eta, iBLayerSplit);
    }
    if (trk.summaryValue(iPixelOutliers, xAOD::numberOfPixelOutliers)) {
      fillHisto(nPixelOutliers, iPixelOutliers);
      fillHisto(n_vs_eta_PixelOutliers, eta, iPixelOutliers);
    }
    if (trk.summaryValue(iPixelContribLayers, xAOD::numberOfContribPixelLayers)) {
      fillHisto(nPixelContribLayers, iPixelContribLayers);
      fillHisto(n_vs_eta_PixelContribLayers, eta, iPixelContribLayers);
    }
    if (trk.summaryValue(iPixelShared, xAOD::numberOfPixelSharedHits)) {
      fillHisto(nPixelSharedHits, iPixelShared);
      fillHisto(n_vs_eta_PixelSharedHits, eta, iPixelShared);
    }
    if (trk.summaryValue(iPixelSplit, xAOD::numberOfPixelSplitHits)) {
      fillHisto(nPixelSplitHits, iPixelSplit);
      fillHisto(n_vs_eta_PixelSplitHits, eta, iPixelSplit);
    }
    if (trk.summaryValue(iPixelGanged, xAOD::numberOfGangedPixels)) {
      fillHisto(nPixelGangedHits, iPixelGanged);
      fillHisto(n_vs_eta_PixelGangedHits, eta, iPixelGanged);
    }
    if (trk.summaryValue(iPixelGangedFF, xAOD::numberOfGangedFlaggedFakes)) {
      fillHisto(nPixelGangedHitsFlaggedFakes, iPixelGangedFF);
      fillHisto(n_vs_eta_PixelGangedHitsFlaggedFakes, eta, iPixelGangedFF);
    }
    if (trk.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
      fillHisto(nSCTOutliers, iSCTOutliers);
      fillHisto(n_vs_eta_SCTOutliers, eta, iSCTOutliers);
    }
    if (trk.summaryValue(iSCTDoubleHoles, xAOD::numberOfSCTDoubleHoles)) {
      fillHisto(nSCTDoubleHoles, iSCTDoubleHoles);
      fillHisto(n_vs_eta_SCTDoubleHoles, eta, iSCTDoubleHoles);
    }
    if (trk.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
      fillHisto(nSCTSharedHits, iSCTShared);
      fillHisto(n_vs_eta_SCTSharedHits, eta, iSCTShared);
    }
    if (trk.summaryValue(iTRTOutliers, xAOD::numberOfTRTOutliers)) {
      fillHisto(nTRTOutliers, iTRTOutliers);
      fillHisto(n_vs_eta_TRTOutliers, eta, iTRTOutliers);
    }
    if (trk.summaryValue(iTRTHTOutliers, xAOD::numberOfTRTHighThresholdOutliers)) {
      fillHisto(nTRTHighThresholdOutliers, iTRTHTOutliers);
      fillHisto(n_vs_eta_TRTHighThresholdOutliers, eta, iTRTHTOutliers);
    }
  }
}
