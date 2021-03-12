/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_LARGERADIUSTRACKING
#define INDETPHYSVALMONITORING_INDETPERFPLOT_LARGERADIUSTRACKING

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

// std includes
#include <string>

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_LargeRadiusTracking: public InDetPlotBase {
public:
  InDetPerfPlot_LargeRadiusTracking(InDetPlotBase* pParent, const std::string& dirName);
  
  
  // for fake rate
  void fillFakeRate(const xAOD::TrackParticle& track, const bool isFake, const float mu, const unsigned int nVtx);
  void fillLinkedandUnlinked(const xAOD::TrackParticle& particle, float Unlinked_w, const float mu, const unsigned int nVtx);
  
  // for efficiency
  void fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle& track, const bool isGood, const float mu, const unsigned int nVtx);
  
  // fill
  void fill(const xAOD::TrackParticle& track);
  void fill(const unsigned int nTrkLRT, const unsigned int nTrkSTD, const unsigned int nTrkConv, const float mu, const unsigned int nVtx);
  

private:

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();

  TEfficiency* m_effLRT_eta;
  TEfficiency *m_effSTD_eta;
  TEfficiency *m_effConv_eta;
  TEfficiency *m_effLRT_pt;
  TEfficiency *m_effSTD_pt;
  TEfficiency *m_effConv_pt;
  TEfficiency *m_effLRT_phi;
  TEfficiency *m_effSTD_phi;
  TEfficiency *m_effConv_phi;
  TEfficiency *m_effLRT_d0;
  TEfficiency *m_effSTD_d0;
  TEfficiency *m_effConv_d0;
  TEfficiency *m_effLRT_z0;
  TEfficiency *m_effSTD_z0;
  TEfficiency *m_effConv_z0;
  TEfficiency *m_effLRT_radius;
  TEfficiency *m_effSTD_radius;
  TEfficiency *m_effConv_radius;
  TEfficiency *m_effLRT_mu;
  TEfficiency *m_effSTD_mu;
  TEfficiency *m_effConv_mu;
  TEfficiency *m_effLRT_nvertices;
  TEfficiency *m_effSTD_nvertices;
  TEfficiency *m_effConv_nvertices;
  
  
  TEfficiency *m_fakeLRT_eta;
  TEfficiency *m_fakeSTD_eta;
  TEfficiency *m_fakeConv_eta;
  TEfficiency *m_fakeLRT_pt;
  TEfficiency *m_fakeSTD_pt;
  TEfficiency *m_fakeConv_pt;
  TEfficiency *m_fakeLRT_phi;
  TEfficiency *m_fakeSTD_phi;
  TEfficiency *m_fakeConv_phi;
  TEfficiency *m_fakeLRT_d0;
  TEfficiency *m_fakeSTD_d0;
  TEfficiency *m_fakeConv_d0;
  TEfficiency *m_fakeLRT_z0;
  TEfficiency *m_fakeSTD_z0;
  TEfficiency *m_fakeConv_z0;
  TEfficiency *m_fakeLRT_mu;
  TEfficiency *m_fakeSTD_mu;
  TEfficiency *m_fakeConv_mu;
  TEfficiency *m_fakeLRT_nvertices;
  TEfficiency *m_fakeSTD_nvertices;
  TEfficiency *m_fakeConv_nvertices;
  
  TEfficiency *m_unlinkedLRT_eta;
  TEfficiency *m_unlinkedSTD_eta;
  TEfficiency *m_unlinkedConv_eta;
  TEfficiency *m_unlinkedLRT_pt;
  TEfficiency *m_unlinkedSTD_pt;
  TEfficiency *m_unlinkedConv_pt;
  TEfficiency *m_unlinkedLRT_phi;
  TEfficiency *m_unlinkedSTD_phi;
  TEfficiency *m_unlinkedConv_phi;
  TEfficiency *m_unlinkedLRT_d0;
  TEfficiency *m_unlinkedSTD_d0;
  TEfficiency *m_unlinkedConv_d0;
  TEfficiency *m_unlinkedLRT_z0;
  TEfficiency *m_unlinkedSTD_z0;
  TEfficiency *m_unlinkedConv_z0;
  TEfficiency *m_unlinkedLRT_mu;
  TEfficiency *m_unlinkedSTD_mu;
  TEfficiency *m_unlinkedConv_mu;
  TEfficiency *m_unlinkedLRT_nvertices;
  TEfficiency *m_unlinkedSTD_nvertices;
  TEfficiency *m_unlinkedConv_nvertices;
  
  TEfficiency *m_linkedLRT_eta;
  TEfficiency *m_linkedSTD_eta;
  TEfficiency *m_linkedConv_eta;
  TEfficiency *m_linkedLRT_pt;
  TEfficiency *m_linkedSTD_pt;
  TEfficiency *m_linkedConv_pt;
  TEfficiency *m_linkedLRT_phi;
  TEfficiency *m_linkedSTD_phi;
  TEfficiency *m_linkedConv_phi;
  TEfficiency *m_linkedLRT_d0;
  TEfficiency *m_linkedSTD_d0;
  TEfficiency *m_linkedConv_d0;
  TEfficiency *m_linkedLRT_z0;
  TEfficiency *m_linkedSTD_z0;
  TEfficiency *m_linkedConv_z0;
  TEfficiency *m_linkedLRT_mu;
  TEfficiency *m_linkedSTD_mu;
  TEfficiency *m_linkedConv_mu;
  TEfficiency *m_linkedLRT_nvertices;
  TEfficiency *m_linkedSTD_nvertices;
  TEfficiency *m_linkedConv_nvertices;
  
  TH1* m_trkpropLRT_eta;
  TH1* m_trkpropConv_eta;
  TH1* m_trkpropSTD_eta;
  TH1* m_trkpropLRT_phi;
  TH1* m_trkpropConv_phi;
  TH1* m_trkpropSTD_phi;
  TH1* m_trkpropLRT_pt;
  TH1* m_trkpropConv_pt;
  TH1* m_trkpropSTD_pt;
  TH1* m_trkpropLRT_d0;
  TH1* m_trkpropConv_d0;
  TH1* m_trkpropSTD_d0;
  TH1* m_trkpropLRT_z0;
  TH1* m_trkpropConv_z0;
  TH1* m_trkpropSTD_z0;
  
  TH2* m_ntrkLRT_mu;
  TH2* m_ntrkConv_mu;
  TH2* m_ntrkSTD_mu;
  
  TH2* m_ntrkLRT_nvertices;
  TH2* m_ntrkConv_nvertices;
  TH2* m_ntrkSTD_nvertices;
  
  TProfile* m_nPixelHitsLRT_vs_eta;
  TProfile* m_nPixelHitsSTD_vs_eta;
  TProfile* m_nPixelHitsConv_vs_eta;
  
  TProfile* m_nSCTHitsLRT_vs_eta;
  TProfile* m_nSCTHitsSTD_vs_eta;
  TProfile* m_nSCTHitsConv_vs_eta;
  
  TProfile* m_nPixelHolesLRT_vs_eta;
  TProfile* m_nPixelHolesSTD_vs_eta;
  TProfile* m_nPixelHolesConv_vs_eta;
  
  TProfile* m_nSCTHolesLRT_vs_eta;
  TProfile* m_nSCTHolesSTD_vs_eta;
  TProfile* m_nSCTHolesConv_vs_eta;
  
  TProfile* m_nPixelSharedHitsLRT_vs_eta;
  TProfile* m_nPixelSharedHitsSTD_vs_eta;
  TProfile* m_nPixelSharedHitsConv_vs_eta;
  
  TProfile* m_nSCTSharedHitsLRT_vs_eta;
  TProfile* m_nSCTSharedHitsSTD_vs_eta;
  TProfile* m_nSCTSharedHitsConv_vs_eta;
  
};

#endif

