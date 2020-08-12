/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_ANTRACKING
#define INDETPHYSVALMONITORING_INDETPERFPLOT_ANTRACKING
/**
 * @file InDetPerfPlot_ANTracking.cxx
 * @author Gabrel Facin
 * Wed Oct 29 09:58:58 CET 2014
 *
 * a lot of this is copied from ANTrackingPlots in the TrkValHistUtils which is dumb
 * the point is that many instances of this will be created so more control of the names
 * is needed.  I don't have permission for that package and time is short...as usual
 **/



// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

// std includes
#include <string>

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_ANTracking: public InDetPlotBase {
public:
  InDetPerfPlot_ANTracking(InDetPlotBase* pParent, const std::string& dirName);

  void fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle& track, const bool isGood, const float mu, const unsigned int nVtx);
  void fillFakeRate(const xAOD::TrackParticle& track, const bool isAssociatedTruth, const float mu, const unsigned int nVtx);
  void fillUnlinked(const xAOD::TrackParticle& track, const bool isAssociatedTruth, const float mu, const unsigned int nVtx);

  void fill(const xAOD::TrackParticle& track);
  void fill(const unsigned int nTrkANT, const unsigned int nTrkSTD, const unsigned int nTrkBAT, const float mu, const unsigned int nVtx);


private:

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();

  TEfficiency* m_effANT_eta;
  TEfficiency *m_effSTD_eta;
  TEfficiency *m_effBAT_eta;
  TEfficiency *m_effANT_pt;
  TEfficiency *m_effSTD_pt;
  TEfficiency *m_effBAT_pt;
  TEfficiency *m_effANT_phi;
  TEfficiency *m_effSTD_phi;
  TEfficiency *m_effBAT_phi;
  TEfficiency *m_effANT_d0;
  TEfficiency *m_effSTD_d0;
  TEfficiency *m_effBAT_d0;
  TEfficiency *m_effANT_z0;
  TEfficiency *m_effSTD_z0;
  TEfficiency *m_effBAT_z0;
  TEfficiency *m_effANT_radius;
  TEfficiency *m_effSTD_radius;
  TEfficiency *m_effBAT_radius;
  TEfficiency *m_effANT_mu;
  TEfficiency *m_effSTD_mu;
  TEfficiency *m_effBAT_mu;
  TEfficiency *m_effANT_nvertices;
  TEfficiency *m_effSTD_nvertices;
  TEfficiency *m_effBAT_nvertices;


  TEfficiency *m_fakeANT_eta;
  TEfficiency *m_fakeSTD_eta;
  TEfficiency *m_fakeBAT_eta;
  TEfficiency *m_fakeANT_pt;
  TEfficiency *m_fakeSTD_pt;
  TEfficiency *m_fakeBAT_pt;
  TEfficiency *m_fakeANT_phi;
  TEfficiency *m_fakeSTD_phi;
  TEfficiency *m_fakeBAT_phi;
  TEfficiency *m_fakeANT_d0;
  TEfficiency *m_fakeSTD_d0;
  TEfficiency *m_fakeBAT_d0;
  TEfficiency *m_fakeANT_z0;
  TEfficiency *m_fakeSTD_z0;
  TEfficiency *m_fakeBAT_z0;
  TEfficiency *m_fakeANT_mu;
  TEfficiency *m_fakeSTD_mu;
  TEfficiency *m_fakeBAT_mu;
  TEfficiency *m_fakeANT_nvertices;
  TEfficiency *m_fakeSTD_nvertices;
  TEfficiency *m_fakeBAT_nvertices;

  TEfficiency *m_unlinkedANT_eta;
  TEfficiency *m_unlinkedSTD_eta;
  TEfficiency *m_unlinkedBAT_eta;
  TEfficiency *m_unlinkedANT_pt;
  TEfficiency *m_unlinkedSTD_pt;
  TEfficiency *m_unlinkedBAT_pt;
  TEfficiency *m_unlinkedANT_phi;
  TEfficiency *m_unlinkedSTD_phi;
  TEfficiency *m_unlinkedBAT_phi;
  TEfficiency *m_unlinkedANT_d0;
  TEfficiency *m_unlinkedSTD_d0;
  TEfficiency *m_unlinkedBAT_d0;
  TEfficiency *m_unlinkedANT_z0;
  TEfficiency *m_unlinkedSTD_z0;
  TEfficiency *m_unlinkedBAT_z0;
  TEfficiency *m_unlinkedANT_mu;
  TEfficiency *m_unlinkedSTD_mu;
  TEfficiency *m_unlinkedBAT_mu;
  TEfficiency *m_unlinkedANT_nvertices;
  TEfficiency *m_unlinkedSTD_nvertices;
  TEfficiency *m_unlinkedBAT_nvertices;

  TH1* m_trkpropANT_eta;
  TH1* m_trkpropBAT_eta;
  TH1* m_trkpropSTD_eta;
  TH1* m_trkpropANT_phi;
  TH1* m_trkpropBAT_phi;
  TH1* m_trkpropSTD_phi;
  TH1* m_trkpropANT_pt;
  TH1* m_trkpropBAT_pt;
  TH1* m_trkpropSTD_pt;
  TH1* m_trkpropANT_d0;
  TH1* m_trkpropBAT_d0;
  TH1* m_trkpropSTD_d0;
  TH1* m_trkpropANT_z0;
  TH1* m_trkpropBAT_z0;
  TH1* m_trkpropSTD_z0;

  TH2* m_ntrkANT_mu;
  TH2* m_ntrkBAT_mu;
  TH2* m_ntrkSTD_mu;

  TH2* m_ntrkANT_nvertices;
  TH2* m_ntrkBAT_nvertices;
  TH2* m_ntrkSTD_nvertices;

  TProfile* m_nPixelHitsANT_vs_eta;
  TProfile* m_nPixelHitsSTD_vs_eta;
  TProfile* m_nPixelHitsBAT_vs_eta;

  TProfile* m_nSCTHitsANT_vs_eta;
  TProfile* m_nSCTHitsSTD_vs_eta;
  TProfile* m_nSCTHitsBAT_vs_eta;

  TProfile* m_nTRTHitsANT_vs_eta;
  TProfile* m_nTRTHitsSTD_vs_eta;
  TProfile* m_nTRTHitsBAT_vs_eta;

  TProfile* m_nPixelHolesANT_vs_eta;
  TProfile* m_nPixelHolesSTD_vs_eta;
  TProfile* m_nPixelHolesBAT_vs_eta;

  TProfile* m_nSCTHolesANT_vs_eta;
  TProfile* m_nSCTHolesSTD_vs_eta;
  TProfile* m_nSCTHolesBAT_vs_eta;

  TProfile* m_nTRTHTHitsANT_vs_eta;
  TProfile* m_nTRTHTHitsSTD_vs_eta;
  TProfile* m_nTRTHTHitsBAT_vs_eta;

  TProfile* m_nPixelSharedHitsANT_vs_eta;
  TProfile* m_nPixelSharedHitsSTD_vs_eta;
  TProfile* m_nPixelSharedHitsBAT_vs_eta;

  TProfile* m_nSCTSharedHitsANT_vs_eta;
  TProfile* m_nSCTSharedHitsSTD_vs_eta;
  TProfile* m_nSCTSharedHitsBAT_vs_eta;


};

#endif
