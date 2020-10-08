/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  InDetPerfPlot_TrkInJet(InDetPlotBase* pParent, std::string sDir, bool bookFakeAndEff=true);
  void fill(const xAOD::TrackParticle& trk, const xAOD::Jet& jet);

  void fillEfficiency(const xAOD::TruthParticle& trk, const xAOD::Jet& jet, const bool isEfficient);
  void fillFakeRate(const xAOD::TrackParticle& trk, const xAOD::Jet& jet, const bool isFake);


private:
  void initializePlots();

  TProfile* m_nInnerMostPixelHits_vs_dR;
  TProfile* m_nNextToInnerMostPixelHits_vs_dR;
  TProfile* m_nPixelHits_vs_dR;
  TProfile* m_nSCTHits_vs_dR;
  TProfile* m_nTRTHits_vs_dR;
  TProfile* m_nPixelHoles_vs_dR;
  TProfile* m_nSCTHoles_vs_dR;
  TProfile* m_nTRTHighThresholdHits_vs_dR;
  TProfile* m_nInnerMostPixelOutliers_vs_dR;
  TProfile* m_nInnerMostPixelSharedHits_vs_dR;
  TProfile* m_nInnerMostPixelSplitHits_vs_dR;
  TProfile* m_nExpectedInnerMostPixelHits_vs_dR;
  TProfile* m_nExpectedNextToInnerMostPixelHits_vs_dR;
  TProfile* m_nPixelOutliers_vs_dR;
  TProfile* m_nPixelContribLayers_vs_dR;
  TProfile* m_nPixelSharedHits_vs_dR;
  TProfile* m_nPixelSplitHits_vs_dR;
  TProfile* m_nPixelGangedHits_vs_dR;
  TProfile* m_nPixelGangedHitsFlaggedFakes_vs_dR;
  TProfile* m_nSCTOutliers_vs_dR;
  TProfile* m_nSCTDoubleHoles_vs_dR;
  TProfile* m_nSCTSharedHits_vs_dR;
  TProfile* m_nTRTOutliers_vs_dR;
  TProfile* m_nTRTHighThresholdOutliers_vs_dR;
  TProfile* m_nPixelDeadSensors_vs_dR;
  TProfile* m_nSCTDeadSensors_vs_dR;


  TProfile* m_reco_d0_vs_dR;
  TProfile* m_reco_z0_vs_dR;
  TProfile* m_reco_z0sin_vs_dR;

  TProfile* m_reco_phi_vs_dR;
  TProfile* m_reco_theta_vs_dR;
  TProfile* m_reco_eta_vs_dR;
  TProfile* m_reco_pt_vs_dR;
  TProfile* m_reco_lowpt_vs_dR;

  TProfile* m_reco_chi2Overndof_vs_dR;

  TEfficiency* m_efficiency_vs_dR;
  TEfficiency* m_efficiency_vs_smalldR;
  TEfficiency* m_efficiency_vs_jetpT;

  TEfficiency* m_fakerate_vs_dR;
  TEfficiency* m_fakerate_vs_smalldR;
  TEfficiency* m_fakerate_vs_jetpT;

  bool m_doFakeAndEff;


};

#endif
