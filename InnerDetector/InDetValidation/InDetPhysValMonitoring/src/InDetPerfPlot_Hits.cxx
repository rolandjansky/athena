/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_Hits.cxx
 * @author nora pettersson
 * This was the first performance plot class written in this package, more or less
 * simply as a demonstrator.
 **/

#include "InDetPerfPlot_Hits.h"

InDetPerfPlot_Hits::InDetPerfPlot_Hits(InDetPlotBase* pParent, const std::string& sDir) : InDetPlotBase(pParent, sDir),
  m_nInnerMostPixelHits{},
  m_nNextToInnerMostPixelHits{},
  m_nPixelHits{},
  m_nSCTHits{},
  m_nTRTHits{},
  m_nPixelHoles{},
  m_nSCTHoles{},
  m_nTRTHighThresholdHits{},
  m_nInnerMostPixelOutliers{},
  m_nInnerMostPixelSharedHits{},
  m_nInnerMostPixelSplitHits{},
  m_nExpectedInnerMostPixelHits{},
  m_nExpectedNextToInnerMostPixelHits{},
  m_nPixelOutliers{},
  m_nPixelContribLayers{},
  m_nPixelSharedHits{},
  m_nPixelSplitHits{},
  m_nPixelGangedHits{},
  m_nPixelGangedHitsFlaggedFakes{},
  m_nSCTOutliers{},
  m_nSCTDoubleHoles{},
  m_nSCTSharedHits{},
  m_nTRTOutliers{},
  m_nTRTHighThresholdOutliers{},
  m_nPixelDeadSensors{},
  m_nSCTDeadSensors{},
  //TProfiles
  m_nInnerMostPixelHits_vs_eta{},
  m_nNextToInnerMostPixelHits_vs_eta{},
  m_nPixelHits_vs_eta{},
  m_nSCTHits_vs_eta{},
  m_nTRTHits_vs_eta{},
  m_nPixelHoles_vs_eta{},
  m_nSCTHoles_vs_eta{},
  m_nTRTHighThresholdHits_vs_eta{},
  m_nInnerMostPixelOutliers_vs_eta{},
  m_nInnerMostPixelSharedHits_vs_eta{},
  m_nInnerMostPixelSplitHits_vs_eta{},
  m_nExpectedInnerMostPixelHits_vs_eta{},
  m_nExpectedNextToInnerMostPixelHits_vs_eta{},
  m_nPixelOutliers_vs_eta{},
  m_nPixelContribLayers_vs_eta{},
  m_nPixelSharedHits_vs_eta{},
  m_nPixelSplitHits_vs_eta{},
  m_nPixelGangedHits_vs_eta{},
  m_nPixelGangedHitsFlaggedFakes_vs_eta{},
  m_nSCTOutliers_vs_eta{},
  m_nSCTDoubleHoles_vs_eta{},
  m_nSCTSharedHits_vs_eta{},
  m_nTRTOutliers_vs_eta{},
  m_nTRTHighThresholdOutliers_vs_eta{},
  m_nPixelDeadSensors_vs_eta{},   
  m_nSCTDeadSensors_vs_eta{},
  //TProfiles
  m_nInnerMostPixelHits_vs_phi{},
  m_nNextToInnerMostPixelHits_vs_phi{},
  m_nPixelHits_vs_phi{},
  m_nSCTHits_vs_phi{},
  m_nTRTHits_vs_phi{},
  m_nPixelHoles_vs_phi{},
  m_nSCTHoles_vs_phi{},
  m_nTRTHighThresholdHits_vs_phi{},
  m_nInnerMostPixelOutliers_vs_phi{},
  m_nInnerMostPixelSharedHits_vs_phi{},
  m_nInnerMostPixelSplitHits_vs_phi{},
  m_nExpectedInnerMostPixelHits_vs_phi{},
  m_nExpectedNextToInnerMostPixelHits_vs_phi{},
  m_nPixelOutliers_vs_phi{},
  m_nPixelContribLayers_vs_phi{},
  m_nPixelSharedHits_vs_phi{},
  m_nPixelSplitHits_vs_phi{},
  m_nPixelGangedHits_vs_phi{},
  m_nPixelGangedHitsFlaggedFakes_vs_phi{},
  m_nSCTOutliers_vs_phi{},
  m_nSCTDoubleHoles_vs_phi{},
  m_nSCTSharedHits_vs_phi{},
  m_nTRTOutliers_vs_phi{},
  m_nTRTHighThresholdOutliers_vs_phi{},
  m_nPixelDeadSensors_vs_phi{},   
  m_nSCTDeadSensors_vs_phi{} {
  //Nop

}

void
InDetPerfPlot_Hits::initializePlots() {

  book(m_nInnerMostPixelHits, "nInnerMostPixelHits");
  book(m_nNextToInnerMostPixelHits, "nNextToInnerMostPixelHits");
  book(m_nPixelHits, "nPixelHits");
  book(m_nSCTHits, "nSCTHits");
  book(m_nTRTHits, "nTRTHits");
  book(m_nPixelHoles, "nPixelHoles");
  book(m_nSCTHoles, "nSCTHoles");
  book(m_nTRTHighThresholdHits, "nTRTHighThresholdHits");
  book(m_nInnerMostPixelHits_vs_eta, "nInnerMostPixelHits_vs_eta");
  book(m_nNextToInnerMostPixelHits_vs_eta, "nNextToInnerMostPixelHits_vs_eta");
  book(m_nPixelHits_vs_eta, "nPixelHits_vs_eta");
  book(m_nSCTHits_vs_eta, "nSCTHits_vs_eta");
  book(m_nTRTHits_vs_eta, "nTRTHits_vs_eta");
  book(m_nPixelHoles_vs_eta, "nPixelHoles_vs_eta");
  book(m_nSCTHoles_vs_eta, "nSCTHoles_vs_eta");
  book(m_nTRTHighThresholdHits_vs_eta, "nTRTHighThresholdHits_vs_eta");
  book(m_nInnerMostPixelHits_vs_phi, "nInnerMostPixelHits_vs_phi");
  book(m_nNextToInnerMostPixelHits_vs_phi, "nNextToInnerMostPixelHits_vs_phi");
  book(m_nPixelHits_vs_phi, "nPixelHits_vs_phi");
  book(m_nSCTHits_vs_phi, "nSCTHits_vs_phi");
  book(m_nTRTHits_vs_phi, "nTRTHits_vs_phi");
  book(m_nPixelHoles_vs_phi, "nPixelHoles_vs_phi");
  book(m_nSCTHoles_vs_phi, "nSCTHoles_vs_phi");
  book(m_nTRTHighThresholdHits_vs_phi, "nTRTHighThresholdHits_vs_phi");

  book(m_nInnerMostPixelSharedHits, "nInnerMostPixelSharedHits");
  book(m_nPixelSharedHits, "nPixelSharedHits");
  book(m_nSCTSharedHits, "nSCTSharedHits");
  book(m_nInnerMostPixelSharedHits_vs_eta, "nInnerMostPixelSharedHits_vs_eta");
  book(m_nPixelSharedHits_vs_eta, "nPixelSharedHits_vs_eta");
  book(m_nSCTSharedHits_vs_eta, "nSCTSharedHits_vs_eta");
  book(m_nInnerMostPixelSharedHits_vs_phi, "nInnerMostPixelSharedHits_vs_phi");
  book(m_nPixelSharedHits_vs_phi, "nPixelSharedHits_vs_phi");
  book(m_nSCTSharedHits_vs_phi, "nSCTSharedHits_vs_phi");

  if (m_iDetailLevel >= 100){
    book(m_nInnerMostPixelOutliers, "nInnerMostPixelOutliers");
    book(m_nInnerMostPixelSplitHits, "nInnerMostPixelSplitHits");
    book(m_nExpectedInnerMostPixelHits, "nExpectedInnerMostPixelHits");
    book(m_nExpectedNextToInnerMostPixelHits, "nExpectedNextToInnerMostPixelHits");
    book(m_nPixelOutliers, "nPixelOutliers");
    book(m_nPixelContribLayers, "nPixelContribLayers");
    book(m_nPixelSplitHits, "nPixelSplitHits");
    book(m_nPixelGangedHits, "nPixelGangedHits");
    book(m_nPixelGangedHitsFlaggedFakes, "nPixelGangedHitsFlaggedFakes");
    book(m_nSCTOutliers, "nSCTOutliers");
    book(m_nSCTDoubleHoles, "nSCTDoubleHoles");
    book(m_nTRTOutliers, "nTRTOutliers");
    book(m_nTRTHighThresholdOutliers, "nTRTHighThresholdOutliers");
    book(m_nPixelDeadSensors, "nPixelDeadSensors");
    book(m_nSCTDeadSensors, "nSCTDeadSensors");
  
    book(m_nInnerMostPixelOutliers_vs_eta, "nInnerMostPixelOutliers_vs_eta");
    book(m_nInnerMostPixelSplitHits_vs_eta, "nInnerMostPixelSplitHits_vs_eta");
    book(m_nExpectedInnerMostPixelHits_vs_eta, "nExpectedInnerMostPixelHits_vs_eta");
    book(m_nExpectedNextToInnerMostPixelHits_vs_eta, "nExpectedNextToInnerMostPixelHits_vs_eta");
    book(m_nPixelOutliers_vs_eta, "nPixelOutliers_vs_eta");
    book(m_nPixelContribLayers_vs_eta, "nPixelContribLayers_vs_eta");
    book(m_nPixelSplitHits_vs_eta, "nPixelSplitHits_vs_eta");
    book(m_nPixelGangedHits_vs_eta, "nPixelGangedHits_vs_eta");
    book(m_nPixelGangedHitsFlaggedFakes_vs_eta, "nPixelGangedHitsFlaggedFakes_vs_eta");
    book(m_nSCTOutliers_vs_eta, "nSCTOutliers_vs_eta");
    book(m_nSCTDoubleHoles_vs_eta, "nSCTDoubleHoles_vs_eta");
    book(m_nTRTOutliers_vs_eta, "nTRTOutliers_vs_eta");
    book(m_nTRTHighThresholdOutliers_vs_eta, "nTRTHighThresholdOutliers_vs_eta");
    book(m_nPixelDeadSensors_vs_eta, "nPixelDeadSensors_vs_eta");
    book(m_nSCTDeadSensors_vs_eta, "nSCTDeadSensors_vs_eta");

    book(m_nInnerMostPixelOutliers_vs_phi, "nInnerMostPixelOutliers_vs_phi");
    book(m_nInnerMostPixelSplitHits_vs_phi, "nInnerMostPixelSplitHits_vs_phi");
    book(m_nExpectedInnerMostPixelHits_vs_phi, "nExpectedInnerMostPixelHits_vs_phi");
    book(m_nExpectedNextToInnerMostPixelHits_vs_phi, "nExpectedNextToInnerMostPixelHits_vs_phi");
    book(m_nPixelOutliers_vs_phi, "nPixelOutliers_vs_phi");
    book(m_nPixelContribLayers_vs_phi, "nPixelContribLayers_vs_phi");
    book(m_nPixelSplitHits_vs_phi, "nPixelSplitHits_vs_phi");
    book(m_nPixelGangedHits_vs_phi, "nPixelGangedHits_vs_phi");
    book(m_nPixelGangedHitsFlaggedFakes_vs_phi, "nPixelGangedHitsFlaggedFakes_vs_phi");
    book(m_nSCTOutliers_vs_phi, "nSCTOutliers_vs_phi");
    book(m_nSCTDoubleHoles_vs_phi, "nSCTDoubleHoles_vs_phi");
    book(m_nTRTOutliers_vs_phi, "nTRTOutliers_vs_phi");
    book(m_nTRTHighThresholdOutliers_vs_phi, "nTRTHighThresholdOutliers_vs_phi");
    book(m_nPixelDeadSensors_vs_phi, "nPixelDeadSensors_vs_phi");
    book(m_nSCTDeadSensors_vs_phi, "nSCTDeadSensors_vs_phi");

  }

}

void
InDetPerfPlot_Hits::fill(const xAOD::TrackParticle& track, float weight) {

  float eta = track.eta();
  float phi = track.phi();

  uint8_t iInnerMostPixelHits(0), iNextToInnerMostPixelHits(0), iPixHits(0), iSctHits(0), iTrtHits(0);
  uint8_t iPixHoles(0), iSCTHoles(0), iTrtHTHits(0);
  uint8_t iInnerMostPixelShared(0), iPixelShared(0), iSCTShared(0); 

  if (track.summaryValue(iInnerMostPixelHits, xAOD::numberOfInnermostPixelLayerHits)) {
    fillHisto(m_nInnerMostPixelHits, iInnerMostPixelHits, weight);
    fillHisto(m_nInnerMostPixelHits_vs_eta, eta, iInnerMostPixelHits, weight);
    fillHisto(m_nInnerMostPixelHits_vs_phi, phi, iInnerMostPixelHits, weight);
  }
  if (track.summaryValue(iNextToInnerMostPixelHits, xAOD::numberOfNextToInnermostPixelLayerHits)) {
    fillHisto(m_nNextToInnerMostPixelHits, iNextToInnerMostPixelHits, weight);
    fillHisto(m_nNextToInnerMostPixelHits_vs_eta, eta, iNextToInnerMostPixelHits, weight);
    fillHisto(m_nNextToInnerMostPixelHits_vs_phi, phi, iNextToInnerMostPixelHits, weight);
  }
  if (track.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    fillHisto(m_nPixelHits, iPixHits, weight);
    fillHisto(m_nPixelHits_vs_eta, eta, iPixHits, weight);
    fillHisto(m_nPixelHits_vs_phi, phi, iPixHits, weight);
  }
  if (track.summaryValue(iSctHits, xAOD::numberOfSCTHits)) {
    fillHisto(m_nSCTHits, iSctHits, weight);
    fillHisto(m_nSCTHits_vs_eta, eta, iSctHits, weight);
    fillHisto(m_nSCTHits_vs_phi, phi, iSctHits, weight);
  }
  if (track.summaryValue(iTrtHits, xAOD::numberOfTRTHits)) {
    fillHisto(m_nTRTHits, iTrtHits, weight);
    fillHisto(m_nTRTHits_vs_eta, eta, iTrtHits, weight);
    fillHisto(m_nTRTHits_vs_phi, phi, iTrtHits, weight);
  }
  if (track.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    fillHisto(m_nPixelHoles, iPixHoles, weight);
    fillHisto(m_nPixelHoles_vs_eta, eta, iPixHoles, weight);
    fillHisto(m_nPixelHoles_vs_phi, phi, iPixHoles, weight);
  }
  if (track.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    fillHisto(m_nSCTHoles, iSCTHoles, weight);
    fillHisto(m_nSCTHoles_vs_eta, eta, iSCTHoles, weight);
    fillHisto(m_nSCTHoles_vs_phi, phi, iSCTHoles, weight);
  }
  if (track.summaryValue(iTrtHTHits, xAOD::numberOfTRTHighThresholdHits)) {
    fillHisto(m_nTRTHighThresholdHits, iTrtHTHits, weight);
    fillHisto(m_nTRTHighThresholdHits_vs_eta, eta, iTrtHTHits, weight);
    fillHisto(m_nTRTHighThresholdHits_vs_phi, phi, iTrtHTHits, weight);
  }
  if (track.summaryValue(iInnerMostPixelShared, xAOD::numberOfInnermostPixelLayerSharedHits)) {
    fillHisto(m_nInnerMostPixelSharedHits, iInnerMostPixelShared, weight);
    fillHisto(m_nInnerMostPixelSharedHits_vs_eta, eta, iInnerMostPixelShared, weight);
    fillHisto(m_nInnerMostPixelSharedHits_vs_phi, phi, iInnerMostPixelShared, weight);
  }
  if (track.summaryValue(iPixelShared, xAOD::numberOfPixelSharedHits)) {
    fillHisto(m_nPixelSharedHits, iPixelShared, weight);
    fillHisto(m_nPixelSharedHits_vs_eta, eta, iPixelShared, weight);
    fillHisto(m_nPixelSharedHits_vs_phi, phi, iPixelShared, weight);
  }
  if (track.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
    fillHisto(m_nSCTSharedHits, iSCTShared, weight);
    fillHisto(m_nSCTSharedHits_vs_eta, eta, iSCTShared, weight);
    fillHisto(m_nSCTSharedHits_vs_phi, phi, iSCTShared, weight);
  }

  // expert plots
  if (m_iDetailLevel >= 100) {
    uint8_t iInnerMostPixelOutliers(0), iInnerMostPixelSplit(0);
    uint8_t iExpectedInnerMostPixelHits(0), iExpectedNextToInnerMostPixelHits(0);
    uint8_t iPixelOutliers(0), iPixelContribLayers(0), iPixelSplit(0), iPixelGanged(0), iPixelGangedFakes(0);
    uint8_t iSCTOutliers(0), iSCTDoubleHoles(0); 
    uint8_t iTRTOutliers(0), iTRTHTOutliers(0);
    uint8_t iPixelDeadSensors(0), iSCTDeadSensors(0);

    if (track.summaryValue(iInnerMostPixelOutliers, xAOD::numberOfInnermostPixelLayerOutliers)) {
      fillHisto(m_nInnerMostPixelOutliers, iInnerMostPixelOutliers, weight);
      fillHisto(m_nInnerMostPixelOutliers_vs_eta, eta, iInnerMostPixelOutliers, weight);
      fillHisto(m_nInnerMostPixelOutliers_vs_phi, phi, iInnerMostPixelOutliers, weight);
    }
    if (track.summaryValue(iInnerMostPixelSplit, xAOD::numberOfInnermostPixelLayerSplitHits)) {
      fillHisto(m_nInnerMostPixelSplitHits, iInnerMostPixelSplit, weight);
      fillHisto(m_nInnerMostPixelSplitHits_vs_eta, eta, iInnerMostPixelSplit, weight);
      fillHisto(m_nInnerMostPixelSplitHits_vs_phi, phi, iInnerMostPixelSplit, weight);
    }
    if (track.summaryValue(iExpectedInnerMostPixelHits, xAOD::expectInnermostPixelLayerHit)) {
      fillHisto(m_nExpectedInnerMostPixelHits, iExpectedInnerMostPixelHits, weight);
      fillHisto(m_nExpectedInnerMostPixelHits_vs_eta, eta, iExpectedInnerMostPixelHits, weight);
      fillHisto(m_nExpectedInnerMostPixelHits_vs_phi, phi, iExpectedInnerMostPixelHits, weight);
    }
    if (track.summaryValue(iExpectedNextToInnerMostPixelHits, xAOD::expectNextToInnermostPixelLayerHit)) {
      fillHisto(m_nExpectedNextToInnerMostPixelHits, iExpectedNextToInnerMostPixelHits, weight);
      fillHisto(m_nExpectedNextToInnerMostPixelHits_vs_eta, eta, iExpectedNextToInnerMostPixelHits, weight);
      fillHisto(m_nExpectedNextToInnerMostPixelHits_vs_phi, phi, iExpectedNextToInnerMostPixelHits, weight);
    }
    if (track.summaryValue(iPixelOutliers, xAOD::numberOfPixelOutliers)) {
      fillHisto(m_nPixelOutliers, iPixelOutliers, weight);
      fillHisto(m_nPixelOutliers_vs_eta, eta, iPixelOutliers, weight);
      fillHisto(m_nPixelOutliers_vs_phi, phi, iPixelOutliers, weight);
    }
    if (track.summaryValue(iPixelContribLayers, xAOD::numberOfContribPixelLayers)) {
      fillHisto(m_nPixelContribLayers, iPixelContribLayers, weight);
      fillHisto(m_nPixelContribLayers_vs_eta, eta, iPixelContribLayers, weight);
      fillHisto(m_nPixelContribLayers_vs_phi, phi, iPixelContribLayers, weight);
    }
    if (track.summaryValue(iPixelSplit, xAOD::numberOfPixelSplitHits)) {
      fillHisto(m_nPixelSplitHits, iPixelSplit, weight);
      fillHisto(m_nPixelSplitHits_vs_eta, eta, iPixelSplit, weight);
      fillHisto(m_nPixelSplitHits_vs_phi, phi, iPixelSplit, weight);
    }
    if (track.summaryValue(iPixelGanged, xAOD::numberOfGangedPixels)) {
      fillHisto(m_nPixelGangedHits, iPixelGanged, weight);
      fillHisto(m_nPixelGangedHits_vs_eta, eta, iPixelGanged, weight);
      fillHisto(m_nPixelGangedHits_vs_phi, phi, iPixelGanged, weight);
    }
    if (track.summaryValue(iPixelGangedFakes, xAOD::numberOfGangedFlaggedFakes)) {
      fillHisto(m_nPixelGangedHitsFlaggedFakes, iPixelGangedFakes, weight);
      fillHisto(m_nPixelGangedHitsFlaggedFakes_vs_eta, eta, iPixelGangedFakes, weight);
      fillHisto(m_nPixelGangedHitsFlaggedFakes_vs_phi, phi, iPixelGangedFakes, weight);
    }
    if (track.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
      fillHisto(m_nSCTOutliers, iSCTOutliers, weight);
      fillHisto(m_nSCTOutliers_vs_eta, eta, iSCTOutliers, weight);
      fillHisto(m_nSCTOutliers_vs_phi, phi, iSCTOutliers, weight);
    }
    if (track.summaryValue(iSCTDoubleHoles, xAOD::numberOfSCTDoubleHoles)) {
      fillHisto(m_nSCTDoubleHoles, iSCTDoubleHoles, weight);
      fillHisto(m_nSCTDoubleHoles_vs_eta, eta, iSCTDoubleHoles, weight);
      fillHisto(m_nSCTDoubleHoles_vs_phi, phi, iSCTDoubleHoles, weight);
    }
    if (track.summaryValue(iTRTOutliers, xAOD::numberOfTRTOutliers)) {
      fillHisto(m_nTRTOutliers, iTRTOutliers, weight);
      fillHisto(m_nTRTOutliers_vs_eta, eta, iTRTOutliers, weight);
      fillHisto(m_nTRTOutliers_vs_phi, phi, iTRTOutliers, weight);
    }
    if (track.summaryValue(iTRTHTOutliers, xAOD::numberOfTRTHighThresholdOutliers)) {
      fillHisto(m_nTRTHighThresholdOutliers, iTRTHTOutliers, weight);
      fillHisto(m_nTRTHighThresholdOutliers_vs_eta, eta, iTRTHTOutliers, weight);
      fillHisto(m_nTRTHighThresholdOutliers_vs_phi, phi, iTRTHTOutliers, weight);
    }
    if (track.summaryValue(iPixelDeadSensors, xAOD::numberOfPixelDeadSensors)) {
      fillHisto(m_nPixelDeadSensors, iPixelDeadSensors, weight);
      fillHisto(m_nPixelDeadSensors_vs_eta, eta, iPixelDeadSensors, weight);
      fillHisto(m_nPixelDeadSensors_vs_phi, phi, iPixelDeadSensors, weight);
    }
    if (track.summaryValue(iSCTDeadSensors, xAOD::numberOfSCTDeadSensors)) {
      fillHisto(m_nSCTDeadSensors, iSCTDeadSensors, weight);
      fillHisto(m_nSCTDeadSensors_vs_eta, eta, iSCTDeadSensors, weight);
      fillHisto(m_nSCTDeadSensors_vs_phi, phi, iSCTDeadSensors, weight);
    }

  }

}

