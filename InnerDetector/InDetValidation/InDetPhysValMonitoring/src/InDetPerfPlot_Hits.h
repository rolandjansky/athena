/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_HITS
#define INDETPHYSVALMONITORING_INDETPERFPLOT_HITS
/**
 * @file InDetPerfPlot_Hits.h
 * @author shaun roe
 **/


// std includes
#include <string>

// local includes
#include "InDetPlotBase.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"


///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_Hits: public InDetPlotBase {
public:
  InDetPerfPlot_Hits(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& track);
private:
  //
  //1D
  //
  TH1* m_nInnerMostPixelHits;
  TH1* m_nNextToInnerMostPixelHits;
  TH1* m_nPixelHits;
  TH1* m_nSCTHits;
  TH1* m_nTRTHits;
  TH1* m_nPixelHoles;
  TH1* m_nSCTHoles;
  TH1* m_nTRTHighThresholdHits;
  TH1* m_nInnerMostPixelOutliers;
  TH1* m_nInnerMostPixelSharedHits;
  TH1* m_nInnerMostPixelSplitHits;
  TH1* m_nExpectedInnerMostPixelHits;
  TH1* m_nExpectedNextToInnerMostPixelHits;
  TH1* m_nPixelOutliers;
  TH1* m_nPixelContribLayers;
  TH1* m_nPixelSharedHits;
  TH1* m_nPixelSplitHits;
  TH1* m_nPixelGangedHits;
  TH1* m_nPixelGangedHitsFlaggedFakes;
  TH1* m_nSCTOutliers;
  TH1* m_nSCTDoubleHoles;
  TH1* m_nSCTSharedHits;
  TH1* m_nTRTOutliers;
  TH1* m_nTRTHighThresholdOutliers;
  TH1* m_nPixelDeadSensors;
  TH1* m_nSCTDeadSensors;
  //
  //TProfiles versus eta
  //
  TProfile* m_nInnerMostPixelHits_vs_eta;
  TProfile* m_nNextToInnerMostPixelHits_vs_eta;
  TProfile* m_nPixelHits_vs_eta;
  TProfile* m_nSCTHits_vs_eta;
  TProfile* m_nTRTHits_vs_eta;
  TProfile* m_nPixelHoles_vs_eta;
  TProfile* m_nSCTHoles_vs_eta;
  TProfile* m_nTRTHighThresholdHits_vs_eta;
  TProfile* m_nInnerMostPixelOutliers_vs_eta;
  TProfile* m_nInnerMostPixelSharedHits_vs_eta;
  TProfile* m_nInnerMostPixelSplitHits_vs_eta;
  TProfile* m_nExpectedInnerMostPixelHits_vs_eta;
  TProfile* m_nExpectedNextToInnerMostPixelHits_vs_eta;
  TProfile* m_nPixelOutliers_vs_eta;
  TProfile* m_nPixelContribLayers_vs_eta;
  TProfile* m_nPixelSharedHits_vs_eta;
  TProfile* m_nPixelSplitHits_vs_eta;
  TProfile* m_nPixelGangedHits_vs_eta;
  TProfile* m_nPixelGangedHitsFlaggedFakes_vs_eta;
  TProfile* m_nSCTOutliers_vs_eta;
  TProfile* m_nSCTDoubleHoles_vs_eta;
  TProfile* m_nSCTSharedHits_vs_eta;
  TProfile* m_nTRTOutliers_vs_eta;
  TProfile* m_nTRTHighThresholdOutliers_vs_eta;
  TProfile* m_nPixelDeadSensors_vs_eta;
  TProfile* m_nSCTDeadSensors_vs_eta;
  //
  //TProfiles versus phi
  //
  TProfile* m_nInnerMostPixelHits_vs_phi;
  TProfile* m_nNextToInnerMostPixelHits_vs_phi;
  TProfile* m_nPixelHits_vs_phi;
  TProfile* m_nSCTHits_vs_phi;
  TProfile* m_nTRTHits_vs_phi;
  TProfile* m_nPixelHoles_vs_phi;
  TProfile* m_nSCTHoles_vs_phi;
  TProfile* m_nTRTHighThresholdHits_vs_phi;
  TProfile* m_nInnerMostPixelOutliers_vs_phi;
  TProfile* m_nInnerMostPixelSharedHits_vs_phi;
  TProfile* m_nInnerMostPixelSplitHits_vs_phi;
  TProfile* m_nExpectedInnerMostPixelHits_vs_phi;
  TProfile* m_nExpectedNextToInnerMostPixelHits_vs_phi;
  TProfile* m_nPixelOutliers_vs_phi;
  TProfile* m_nPixelContribLayers_vs_phi;
  TProfile* m_nPixelSharedHits_vs_phi;
  TProfile* m_nPixelSplitHits_vs_phi;
  TProfile* m_nPixelGangedHits_vs_phi;
  TProfile* m_nPixelGangedHitsFlaggedFakes_vs_phi;
  TProfile* m_nSCTOutliers_vs_phi;
  TProfile* m_nSCTDoubleHoles_vs_phi;
  TProfile* m_nSCTSharedHits_vs_phi;
  TProfile* m_nTRTOutliers_vs_phi;
  TProfile* m_nTRTHighThresholdOutliers_vs_phi;
  TProfile* m_nPixelDeadSensors_vs_phi;
  TProfile* m_nSCTDeadSensors_vs_phi;


  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};




#endif
