/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValHistUtils/IDHitPlots.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Trk {
  void
  IDHitPlots::init() {
    nBLayerHits = nullptr;
    nPixelHits = nullptr;
    nPixelHoles = nullptr;
    nSCTHits = nullptr;
    nSCTHoles = nullptr;
    nTRTHits = nullptr;
    nTRTHighThresholdHits = nullptr;

    nBLayerOutliers = nullptr;
    nBLayerSharedHits = nullptr;
    nBLayerSplitHits = nullptr;
    nPixelOutliers = nullptr;
    nPixelContribLayers = nullptr;
    nPixelSharedHits = nullptr;
    nPixelSplitHits = nullptr;
    nPixelGangedHits = nullptr;
    nSCTOutliers = nullptr;
    nSCTDoubleHoles = nullptr;
    nSCTSharedHits = nullptr;
    nTRTOutliers = nullptr;
    nTRTHighThresholdOutliers = nullptr;
    nPixelGangedHitsFlaggedFakes = nullptr;
  }

  void
  IDHitPlots::initializePlots() {
    nBLayerHits = Book1D("HitContent_NBlayerHits", "Number of B-Layer clusters;Number of Clusters;Entries", 5, -0.5,
                         4.5, false);
    nPixelHits = Book1D("HitContent_NPixelHits", "Number of Pixel clusters;Number of Clusters", 10, -0.5, 9.5, false);
    nPixelHoles = Book1D("HitContent_NPixelHoles", "Number of Pixel holes;Number of Holes", 10, -0.5, 9.5, false);
    nSCTHits = Book1D("HitContent_NSCTHits", "Number of SCT clusters;Number of Clusters.", 21, -0.5, 20.5, false);
    nSCTHoles = Book1D("HitContent_NSCTHoles", "Number of SCT holes;Number of Holes", 21, -0.5, 20.5, false);
    nTRTHits = Book1D("HitContent_NTRTHits", "Number of TRT clusters;Number of Hits", 51, -0.5, 50.5, false);
    nTRTHighThresholdHits = Book1D("HitContent_NTRTHighThresholdHits",
                                   "Number of TRT high threshold Hits;Number of Hits", 51, -0.5, 50.5, false);

    // debug plots
    if (m_iDetailLevel >= 100) {
      nBLayerOutliers = Book1D("HitContent_NBlayerOutliers", "Number of B-layer outliers;Number of Outliers;Entries", 5,
                               -0.5, 4.5, false);
      ;
      nBLayerSharedHits = Book1D("HitContent_NBlayerSharedHits",
                                 "Number of shared B-layer clusters;Number of Shared Clusters", 5, -0.5, 4.5, false);
      nBLayerSplitHits = Book1D("HitContent_NBlayerSplitHits",
                                "Number of split B-layer clusters;Number of Split Clusters", 4, -0.5, 2.5, false);
      nPixelOutliers = Book1D("HitContent_NPixelOutliers", "Number of Pixel outliers;Number of Outliers", 10, -0.5, 9.5,
                              false);
      nPixelContribLayers = Book1D("HitContent_NPixelContribLayers",
                                   "Number of contributed Pixel layers;Number of Layers", 5, -0.5, 4.5, false);
      nPixelSharedHits = Book1D("HitContent_NPixelSharedHits",
                                "Number of shared Pixel clusters;Number of Shared Clusters", 10, -0.5, 9.5, false);
      nPixelSplitHits = Book1D("HitContent_NPixelSplitHits", "Number of split Pixel clusters; Number of Split Clusters",
                               5, -0.5, 4.5, false);
      nPixelGangedHits = Book1D("HitContent_NPixelGangedHits", "Number of ganged Pixel clusters;Number of Clusters", 10,
                                -0.5, 9.5, false);
      nPixelGangedHitsFlaggedFakes = Book1D("HitContent_NPixelGangedHitsFlaggedFakes",
                                            "Number of ganged flagged fake Pixel clusters;Number of Clusters", 10, -0.5,
                                            9.5, false);
      nSCTOutliers = Book1D("HitContent_NSCTOutliers", "Number of SCT Outliers;Number of Outliers", 21, -0.5, 20.5,
                            false);
      nSCTDoubleHoles = Book1D("HitContent_NSCTDoubleHoles", "Number of SCT double holes;Number of Double Holes", 21,
                               -0.5, 20.5, false);
      nSCTSharedHits = Book1D("HitContent_NSCTSharedHits", "Number of shared SCT clusters;Number of Shared Clusters",
                              21, -0.5, 20.5, false);
      nTRTOutliers = Book1D("HitContent_NTRTOutliers", "Number of TRT outliers;Number of Outliers", 51, -0.5, 50.5,
                            false);
      nTRTHighThresholdOutliers = Book1D("HitContent_NTRTHighThresholdOutliers",
                                         "Number of TRT High Threshold outliers;Number of Outliers", 51, -0.5, 50.5,
                                         false);
    }
  }

  void
  IDHitPlots::fill(const xAOD::TrackParticle &trk) {
    uint8_t iBLayerHits(0), iPixHits(0), iSctHits(0), iTrtHits(0);
    uint8_t iPixHoles(0), iSCTHoles(0), iTrtHTHits(0);

    if (trk.summaryValue(iBLayerHits, xAOD::numberOfInnermostPixelLayerHits)) {
      nBLayerHits->Fill(iBLayerHits);
    }
    if (trk.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
      nPixelHits->Fill(iPixHits);
    }
    if (trk.summaryValue(iSctHits, xAOD::numberOfSCTHits)) {
      nSCTHits->Fill(iSctHits);
    }
    if (trk.summaryValue(iTrtHits, xAOD::numberOfTRTHits)) {
      nTRTHits->Fill(iTrtHits);
    }
    if (trk.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
      nPixelHoles->Fill(iPixHoles);
    }
    if (trk.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
      nSCTHoles->Fill(iSCTHoles);
    }
    if (trk.summaryValue(iTrtHTHits, xAOD::numberOfTRTHighThresholdHits)) {
      nTRTHighThresholdHits->Fill(iTrtHTHits);
    }

    // expert plots
    if (m_iDetailLevel >= 100) {
      uint8_t iBLayerOutliers(0), iBLayerShared(0), iBLayerSplit(0);
      uint8_t iPixelOutliers(0), iPixelContribLayers(0), iPixelShared(0), iPixelSplit(0), iPixelGanged(0),
      iPixelGangedFakes(0);
      uint8_t iSCTOutliers(0), iSCTDoubleHoles(0), iSCTShared(0);
      uint8_t iTRTOutliers(0), iTRTHTOutliers(0);
      if (trk.summaryValue(iBLayerOutliers, xAOD::numberOfInnermostPixelLayerOutliers)) {
        nBLayerOutliers->Fill(iBLayerOutliers);
      }
      if (trk.summaryValue(iBLayerShared, xAOD::numberOfInnermostPixelLayerSharedHits)) {
        nBLayerSharedHits->Fill(iBLayerShared);
      }
      if (trk.summaryValue(iBLayerSplit, xAOD::numberOfInnermostPixelLayerSplitHits)) {
        nBLayerSplitHits->Fill(iBLayerSplit);
      }
      if (trk.summaryValue(iPixelOutliers, xAOD::numberOfPixelOutliers)) {
        nPixelOutliers->Fill(iPixelOutliers);
      }
      if (trk.summaryValue(iPixelContribLayers, xAOD::numberOfContribPixelLayers)) {
        nPixelContribLayers->Fill(iPixelContribLayers);
      }
      if (trk.summaryValue(iPixelShared, xAOD::numberOfPixelSharedHits)) {
        nPixelSharedHits->Fill(iPixelShared);
      }
      if (trk.summaryValue(iPixelSplit, xAOD::numberOfPixelSplitHits)) {
        nPixelSplitHits->Fill(iPixelSplit);
      }
      if (trk.summaryValue(iPixelGanged, xAOD::numberOfGangedPixels)) {
        nPixelGangedHits->Fill(iPixelGanged);
      }
      if (trk.summaryValue(iPixelGangedFakes, xAOD::numberOfGangedFlaggedFakes)) {
        nPixelGangedHitsFlaggedFakes->Fill(iPixelGangedFakes);
      }
      if (trk.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
        nSCTOutliers->Fill(iSCTOutliers);
      }
      if (trk.summaryValue(iSCTDoubleHoles, xAOD::numberOfSCTDoubleHoles)) {
        nSCTDoubleHoles->Fill(iSCTDoubleHoles);
      }
      if (trk.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
        nSCTSharedHits->Fill(iSCTShared);
      }
      if (trk.summaryValue(iTRTOutliers, xAOD::numberOfTRTOutliers)) {
        nTRTOutliers->Fill(iTRTOutliers);
      }
      if (trk.summaryValue(iTRTHTOutliers, xAOD::numberOfTRTHighThresholdOutliers)) {
        nTRTHighThresholdOutliers->Fill(iTRTHTOutliers);
      }
    }
  }
}
