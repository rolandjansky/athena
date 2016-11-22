/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_TrtTest
#define INDETPHYSVALMONITORING_InDetPerfPlot_TrtTest

/**
 * @file InDetPerfPlot_TrtTest.h
 * @author margaret lutz
 **/

// std includes
#include <string>
#include <array>

// local includes
#include "xAODTracking/TrackParticle.h"
#include "InDetPlotBase.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

class InDetPerfPlot_TrtTest: public InDetPlotBase {
public:
  InDetPerfPlot_TrtTest(InDetPlotBase *pParent, const std::string &dirName);
  void fill(const xAOD::TrackParticle &particle, const xAOD::EventInfo& ei, const xAOD::VertexContainer& pv);
private:
  ///////////////1D Histograms/////////////////////
  ///////////////////general stuff/////////////////
  TH1 *m_mu, *m_nPV;
  TH1 *m_matchProb;
  TH1 *m_trtTubeFrac;
  ///////////////track categories//////////////////
  TH1 *m_trackCategories,*m_slimTrackCategories;

  TH1 *m_pt,*m_pt_ext,*m_pt_trtHit,*m_pt_trtHitNoOut,*m_pt_trtOutlier,*m_pt_trtHitOut,*m_pt_noTrt;
  TH1 *m_eta,*m_eta_ext,*m_eta_trtHit,*m_eta_trtHitNoOut,*m_eta_trtOutlier,*m_eta_trtHitOut,*m_eta_noTrt;
  TH1 *m_phi,*m_phi_ext,*m_phi_trtHit,*m_phi_trtHitNoOut,*m_phi_trtOutlier,*m_phi_trtHitOut,*m_phi_noTrt;
  TH1 *m_d0,*m_d0_ext,*m_d0_trtHit,*m_d0_trtHitNoOut,*m_d0_trtOutlier,*m_d0_trtHitOut,*m_d0_noTrt;
  TH1 *m_z0,*m_z0_ext,*m_z0_trtHit,*m_z0_trtHitNoOut,*m_z0_trtOutlier,*m_z0_trtHitOut,*m_z0_noTrt;
  TH1 *m_tube,*m_tube_ext,*m_tube_trtHit,*m_tube_trtHitNoOut,*m_tube_trtOutlier,*m_tube_trtHitOut,*m_tube_noTrt;
  ///////////split by tube fraction//////////////// ////////////not used at the moment///////////////////
  TH1 *m_pt_tubeHitFrac_05_1,*m_eta_tubeHitFrac_05_1,*m_phi_tubeHitFrac_05_1,*m_d0_tubeHitFrac_05_1,*m_z0_tubeHitFrac_05_1;
  TH1 *m_pt_tubeHitFrac_04_05,*m_eta_tubeHitFrac_04_05,*m_phi_tubeHitFrac_04_05,*m_d0_tubeHitFrac_04_05,*m_z0_tubeHitFrac_04_05;
  TH1 *m_pt_tubeHitFrac_03_04,*m_eta_tubeHitFrac_03_04,*m_phi_tubeHitFrac_03_04,*m_d0_tubeHitFrac_03_04,*m_z0_tubeHitFrac_03_04;
  TH1 *m_pt_tubeHitFrac_0_03,*m_eta_tubeHitFrac_0_03,*m_phi_tubeHitFrac_0_03,*m_d0_tubeHitFrac_0_03,*m_z0_tubeHitFrac_0_03;
  ///////////////2D Histograms/////////////////////
  TH2 *m_pt_vs_nTrtTubeHits,*m_eta_vs_nTrtTubeHits,*m_phi_vs_nTrtTubeHits,*m_d0_vs_nTrtTubeHits,*m_z0_vs_nTrtTubeHits,*m_mu_vs_nTrtTubeHits,*m_nPV_vs_nTrtTubeHits;
  TH2 *m_pt_vs_nTrtHits,*m_eta_vs_nTrtHits,*m_phi_vs_nTrtHits,*m_d0_vs_nTrtHits,*m_z0_vs_nTrtHits,*m_mu_vs_nTrtHits,*m_nPV_vs_nTrtHits;
  TH2 *m_pt_vs_tubeHitFrac,*m_eta_vs_tubeHitFrac,*m_phi_vs_tubeHitFrac,*m_d0_vs_tubeHitFrac,*m_z0_vs_tubeHitFrac,*m_mu_vs_tubeHitFrac,*m_nPV_vs_tubeHitFrac;
  TH2 *m_nTrtHit_vs_nTrtTubeHit, *m_nTrtHit_vs_trtTubeFrac;

  TH2 *m_d0_vs_eta;


  // Plot base has no default implementation of this; we use it to book the histos
  void initializePlots();

  void finalizePlots(); // Use this to create efficiency plots?
};

#endif
