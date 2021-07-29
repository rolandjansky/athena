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
  void fill(const xAOD::TrackParticle& track, float mu, float weight);
private:
  //
  //1D
  //
  TH1* m_nInnerMostPixelHits{nullptr};
  TH1* m_nNextToInnerMostPixelHits{nullptr};
  TH1* m_nPixelHits{nullptr};
  TH1* m_nSCTHits{nullptr};
  TH1* m_nTRTHits{nullptr};
  TH1* m_nPixelHoles{nullptr};
  TH1* m_nSCTHoles{nullptr};
  TH1* m_nTRTHighThresholdHits{nullptr};
  TH1* m_nInnerMostPixelOutliers{nullptr};
  TH1* m_nInnerMostPixelSharedHits{nullptr};
  TH1* m_nInnerMostPixelSplitHits{nullptr};
  TH1* m_nExpectedInnerMostPixelHits{nullptr};
  TH1* m_nExpectedNextToInnerMostPixelHits{nullptr};
  TH1* m_nPixelOutliers{nullptr};
  TH1* m_nPixelContribLayers{nullptr};
  TH1* m_nPixelSharedHits{nullptr};
  TH1* m_nPixelSplitHits{nullptr};
  TH1* m_nPixelGangedHits{nullptr};
  TH1* m_nPixelGangedHitsFlaggedFakes{nullptr};
  TH1* m_nSCTOutliers{nullptr};
  TH1* m_nSCTDoubleHoles{nullptr};
  TH1* m_nSCTSharedHits{nullptr};
  TH1* m_nTRTOutliers{nullptr};
  TH1* m_nTRTHighThresholdOutliers{nullptr};
  TH1* m_nPixelDeadSensors{nullptr};
  TH1* m_nSCTDeadSensors{nullptr};
  //
  //TProfiles versus eta
  //
  TProfile* m_nInnerMostPixelHits_vs_eta{nullptr};
  TProfile* m_nNextToInnerMostPixelHits_vs_eta{nullptr};
  TProfile* m_nPixelHits_vs_eta{nullptr};
  TProfile* m_nSCTHits_vs_eta{nullptr};
  TProfile* m_nTRTHits_vs_eta{nullptr};
  TProfile* m_nPixelHoles_vs_eta{nullptr};
  TProfile* m_nSCTHoles_vs_eta{nullptr};
  TProfile* m_nTRTHighThresholdHits_vs_eta{nullptr};
  TProfile* m_nInnerMostPixelOutliers_vs_eta{nullptr};
  TProfile* m_nInnerMostPixelSharedHits_vs_eta{nullptr};
  TProfile* m_nInnerMostPixelSplitHits_vs_eta{nullptr};
  TProfile* m_nExpectedInnerMostPixelHits_vs_eta{nullptr};
  TProfile* m_nExpectedNextToInnerMostPixelHits_vs_eta{nullptr};
  TProfile* m_nPixelOutliers_vs_eta{nullptr};
  TProfile* m_nPixelContribLayers_vs_eta{nullptr};
  TProfile* m_nPixelSharedHits_vs_eta{nullptr};
  TProfile* m_nPixelSplitHits_vs_eta{nullptr};
  TProfile* m_nPixelGangedHits_vs_eta{nullptr};
  TProfile* m_nPixelGangedHitsFlaggedFakes_vs_eta{nullptr};
  TProfile* m_nSCTOutliers_vs_eta{nullptr};
  TProfile* m_nSCTDoubleHoles_vs_eta{nullptr};
  TProfile* m_nSCTSharedHits_vs_eta{nullptr};
  TProfile* m_nTRTOutliers_vs_eta{nullptr};
  TProfile* m_nTRTHighThresholdOutliers_vs_eta{nullptr};
  TProfile* m_nPixelDeadSensors_vs_eta{nullptr};
  TProfile* m_nSCTDeadSensors_vs_eta{nullptr};
  //
  //TProfiles versus phi
  //
  TProfile* m_nInnerMostPixelHits_vs_phi{nullptr};
  TProfile* m_nNextToInnerMostPixelHits_vs_phi{nullptr};
  TProfile* m_nPixelHits_vs_phi{nullptr};
  TProfile* m_nSCTHits_vs_phi{nullptr};
  TProfile* m_nTRTHits_vs_phi{nullptr};
  TProfile* m_nPixelHoles_vs_phi{nullptr};
  TProfile* m_nSCTHoles_vs_phi{nullptr};
  TProfile* m_nTRTHighThresholdHits_vs_phi{nullptr};
  TProfile* m_nInnerMostPixelOutliers_vs_phi{nullptr};
  TProfile* m_nInnerMostPixelSharedHits_vs_phi{nullptr};
  TProfile* m_nInnerMostPixelSplitHits_vs_phi{nullptr};
  TProfile* m_nExpectedInnerMostPixelHits_vs_phi{nullptr};
  TProfile* m_nExpectedNextToInnerMostPixelHits_vs_phi{nullptr};
  TProfile* m_nPixelOutliers_vs_phi{nullptr};
  TProfile* m_nPixelContribLayers_vs_phi{nullptr};
  TProfile* m_nPixelSharedHits_vs_phi{nullptr};
  TProfile* m_nPixelSplitHits_vs_phi{nullptr};
  TProfile* m_nPixelGangedHits_vs_phi{nullptr};
  TProfile* m_nPixelGangedHitsFlaggedFakes_vs_phi{nullptr};
  TProfile* m_nSCTOutliers_vs_phi{nullptr};
  TProfile* m_nSCTDoubleHoles_vs_phi{nullptr};
  TProfile* m_nSCTSharedHits_vs_phi{nullptr};
  TProfile* m_nTRTOutliers_vs_phi{nullptr};
  TProfile* m_nTRTHighThresholdOutliers_vs_phi{nullptr};
  TProfile* m_nPixelDeadSensors_vs_phi{nullptr};
  TProfile* m_nSCTDeadSensors_vs_phi{nullptr};

  //
  // 2D profile content 
  //
  TProfile2D* m_nInnerMostPixelHits_vs_etaphi{nullptr};
  TProfile2D* m_nExpectedInnerMostPixelHits_vs_etaphi{nullptr};
  TProfile2D* m_nNextToInnerMostPixelHits_vs_etaphi{nullptr};
  TProfile2D* m_nExpectedNextToInnerMostPixelHits_vs_etaphi{nullptr};
  TProfile2D* m_nPixelHits_vs_etaphi{nullptr};
  TProfile2D* m_nPixelHoles_vs_etaphi{nullptr};
  TProfile2D* m_nPixelDeadSensors_vs_etaphi{nullptr};

  TProfile2D* m_nSCTHits_vs_etaphi{nullptr};
  TProfile2D* m_nSCTHoles_vs_etaphi{nullptr};
  TProfile2D* m_nSCTDeadSensors_vs_etaphi{nullptr};
  TProfile2D* m_nSCTDoubleHoles_vs_etaphi{nullptr};
  //
  //TProfiles versus mu
  //
  TProfile* m_nInnerMostPixelHits_vs_mu{nullptr};
  TProfile* m_nNextToInnerMostPixelHits_vs_mu{nullptr};
  TProfile* m_nPixelHits_vs_mu{nullptr};
  TProfile* m_nSCTHits_vs_mu{nullptr};
  TProfile* m_nTRTHits_vs_mu{nullptr};
  TProfile* m_nPixelHoles_vs_mu{nullptr};
  TProfile* m_nSCTHoles_vs_mu{nullptr};
  TProfile* m_nTRTHighThresholdHits_vs_mu{nullptr};
  TProfile* m_nInnerMostPixelOutliers_vs_mu{nullptr};
  TProfile* m_nInnerMostPixelSharedHits_vs_mu{nullptr};
  TProfile* m_nInnerMostPixelSplitHits_vs_mu{nullptr};
  TProfile* m_nExpectedInnerMostPixelHits_vs_mu{nullptr};
  TProfile* m_nExpectedNextToInnerMostPixelHits_vs_mu{nullptr};
  TProfile* m_nPixelOutliers_vs_mu{nullptr};
  TProfile* m_nPixelContribLayers_vs_mu{nullptr};
  TProfile* m_nPixelSharedHits_vs_mu{nullptr};
  TProfile* m_nPixelSplitHits_vs_mu{nullptr};
  TProfile* m_nPixelGangedHits_vs_mu{nullptr};
  TProfile* m_nPixelGangedHitsFlaggedFakes_vs_mu{nullptr};
  TProfile* m_nSCTOutliers_vs_mu{nullptr};
  TProfile* m_nSCTDoubleHoles_vs_mu{nullptr};
  TProfile* m_nSCTSharedHits_vs_mu{nullptr};
  TProfile* m_nTRTOutliers_vs_mu{nullptr};
  TProfile* m_nTRTHighThresholdOutliers_vs_mu{nullptr};
  TProfile* m_nPixelDeadSensors_vs_mu{nullptr};
  TProfile* m_nSCTDeadSensors_vs_mu{nullptr};


  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};




#endif
