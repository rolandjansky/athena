/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetPerfPlot_HitDetailed.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"



InDetPerfPlot_HitDetailed::InDetPerfPlot_HitDetailed(InDetPlotBase *pParent, std::string sDir):InDetPlotBase(pParent, sDir),
n_vs_eta_BLayerHits{},
n_vs_eta_BLayerOutliers{},
n_vs_eta_BLayerSharedHits{},
n_vs_eta_BLayerSplitHits{},
n_vs_eta_DBMHitsNeg{},
n_vs_eta_DBMHitsPos{},
n_vs_eta_PixelHits{},
n_vs_eta_PixelHoles{},
n_vs_eta_PixelOutliers{},
n_vs_eta_PixelContribLayers{},
n_vs_eta_PixelSharedHits{},
n_vs_eta_PixelSplitHits{},
n_vs_eta_PixelGangedHits{},
n_vs_eta_SCTHits{},
n_vs_eta_SCTHoles{},
n_vs_eta_SCTOutliers{},
n_vs_eta_SCTDoubleHoles{},
n_vs_eta_SCTSharedHits{},
n_vs_eta_TRTHits{},
n_vs_eta_TRTHighThresholdHits{},
n_vs_eta_TRTOutliers{},
n_vs_eta_TRTHighThresholdOutliers{}
 {
  m_etaNBins = 20;
  m_etaMin = -2.5;
  m_etaMax = 2.5;
  m_etaMinDBM = 2.5;
  m_etaMaxDBM = 3.8;
}

void InDetPerfPlot_HitDetailed::setEtaBinning(int nbins, float eta_min, float eta_max) {
  m_etaNBins = nbins;
  m_etaMin = eta_min;
  m_etaMax = eta_max;
}

void InDetPerfPlot_HitDetailed::initializePlots(){  
  book(n_vs_eta_BLayerHits,"HitContent_vs_eta_NBlayerHits");
  book(n_vs_eta_PixelHits,"HitContent_vs_eta_NPixelHits");
  book(n_vs_eta_DBMHitsNeg,"HitContent_vs_eta_DBMHitsNeg");
  book(n_vs_eta_DBMHitsPos,"HitContent_vs_eta_DBMHitsPos");
  book(n_vs_eta_PixelHoles,"HitContent_vs_eta_NPixelHoles");
  book(n_vs_eta_SCTHits,"HitContent_vs_eta_NSCTHits");
  book(n_vs_eta_SCTHoles,"HitContent_vs_eta_NSCTHoles");
  book(n_vs_eta_TRTHits,"HitContent_vs_eta_NTRTHits"); 
  book(n_vs_eta_TRTHighThresholdHits,"HitContent_vs_eta_NTRTHighThresholdHits");
  if (m_iDetailLevel >= 100) {
    book(n_vs_eta_BLayerOutliers,"HitContent_vs_eta_NBlayerOutliers");
    book(n_vs_eta_BLayerSharedHits,"HitContent_vs_eta_NBlayerSharedHits");
    book(n_vs_eta_BLayerSplitHits,"HitContent_vs_eta_NBLayerSplitHits");
    book(n_vs_eta_PixelOutliers,"HitContent_vs_eta_NPixelOutliers");
    book(n_vs_eta_PixelContribLayers,"HitContent_vs_eta_NPixelContribLayers");
    book(n_vs_eta_PixelSharedHits,"HitContent_vs_eta_NPixelSharedHits");
    book(n_vs_eta_PixelSplitHits,"HitContent_vs_eta_NPixelSplitHits");
    book(n_vs_eta_PixelGangedHits,"HitContent_vs_eta_NPixelGangedHits");
    book(n_vs_eta_SCTOutliers,"HitContent_vs_eta_NSCTOutliers");
    book(n_vs_eta_SCTDoubleHoles,"HitContent_vs_eta_NSCTDoubleHoles");
    book(n_vs_eta_SCTSharedHits,"HitContent_vs_eta_NSCTSharedHits");
    book(n_vs_eta_TRTOutliers,"HitContent_vs_eta_NTRTOutliers");
    book(n_vs_eta_TRTHighThresholdOutliers,"HitContent_vs_eta_NTRTHighThresholdOutliers");
  }
}

void InDetPerfPlot_HitDetailed::fill(const xAOD::TrackParticle& trk) {

  float eta = trk.eta();

  uint8_t iBLayerHits(0),iPixHits(0),iSctHits(0),iTrtHits(0);
  uint8_t iPixHoles(0),iSCTHoles(0),iTrtHTHits(0);  

  if (trk.summaryValue(iBLayerHits,xAOD::numberOfBLayerHits)) n_vs_eta_BLayerHits->Fill(eta, iBLayerHits);
  if (trk.summaryValue(iPixHits,xAOD::numberOfPixelHits))     n_vs_eta_PixelHits->Fill(eta, iPixHits);
  if (trk.summaryValue(iSctHits,xAOD::numberOfSCTHits))       n_vs_eta_SCTHits->Fill(eta, iSctHits);
  if (trk.summaryValue(iTrtHits,xAOD::numberOfTRTHits))       n_vs_eta_TRTHits->Fill(eta, iTrtHits);
  if (trk.summaryValue(iPixHoles,xAOD::numberOfPixelHoles))   n_vs_eta_PixelHoles->Fill(eta, iPixHoles);
  if (trk.summaryValue(iSCTHoles,xAOD::numberOfSCTHoles))     n_vs_eta_SCTHoles->Fill(eta, iSCTHoles);
  if (trk.summaryValue(iTrtHTHits,xAOD::numberOfTRTHighThresholdHits)) n_vs_eta_TRTHighThresholdHits->Fill(eta, iTrtHTHits);

  //expert plots
  if (m_iDetailLevel >= 100) {
    uint8_t iBLayerOutliers(0),iBLayerShared(0),iBLayerSplit(0);
    uint8_t iPixelOutliers(0),iPixelContribLayers(0),iPixelShared(0),iPixelSplit(0),iPixelGanged(0);
    uint8_t iSCTOutliers(0),iSCTDoubleHoles(0),iSCTShared(0);
    uint8_t iTRTOutliers(0),iTRTHTOutliers(0);
    if (trk.summaryValue(iBLayerOutliers,xAOD::numberOfBLayerOutliers)) n_vs_eta_BLayerOutliers->Fill(eta, iBLayerOutliers);
    if (trk.summaryValue(iBLayerShared,xAOD::numberOfBLayerSharedHits)) n_vs_eta_BLayerSharedHits->Fill(eta, iBLayerShared);
    if (trk.summaryValue(iBLayerSplit,xAOD::numberOfBLayerSplitHits)) n_vs_eta_BLayerSplitHits->Fill(eta, iBLayerSplit);
    if (trk.summaryValue(iPixelOutliers,xAOD::numberOfPixelOutliers)) n_vs_eta_PixelOutliers->Fill(eta, iPixelOutliers);
    if (trk.summaryValue(iPixelContribLayers,xAOD::numberOfContribPixelLayers)) n_vs_eta_PixelContribLayers->Fill(eta, iPixelContribLayers);
    if (trk.summaryValue(iPixelShared,xAOD::numberOfPixelSharedHits)) n_vs_eta_PixelSharedHits->Fill(eta, iPixelShared);
    if (trk.summaryValue(iPixelSplit,xAOD::numberOfPixelSplitHits)) n_vs_eta_PixelSplitHits->Fill(eta, iPixelSplit);
    if (trk.summaryValue(iPixelGanged,xAOD::numberOfGangedPixels)) n_vs_eta_PixelGangedHits->Fill(eta, iPixelGanged);
    if (trk.summaryValue(iSCTOutliers,xAOD::numberOfSCTOutliers)) n_vs_eta_PixelOutliers->Fill(eta, iSCTOutliers);
    if (trk.summaryValue(iSCTDoubleHoles,xAOD::numberOfSCTDoubleHoles)) n_vs_eta_SCTDoubleHoles->Fill(eta, iSCTDoubleHoles);
    if (trk.summaryValue(iSCTShared,xAOD::numberOfSCTSharedHits)) n_vs_eta_SCTSharedHits->Fill(eta, iSCTShared);
    if (trk.summaryValue(iTRTOutliers,xAOD::numberOfTRTOutliers)) n_vs_eta_TRTOutliers->Fill(eta, iTRTOutliers);
    if (trk.summaryValue(iTRTHTOutliers,xAOD::numberOfTRTHighThresholdOutliers)) n_vs_eta_TRTHighThresholdOutliers->Fill(eta, iTRTHTOutliers);
  }
}
