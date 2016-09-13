/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_FAKES
#define INDETPHYSVALMONITORING_INDETPERFPLOT_FAKES
/**
 * @file InDetPerfPlot_fakes.h
 * @author shaun roe
 **/


// std includes
#include <string>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

///class holding fake plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_fakes: public InDetPlotBase {
  friend class InDetPerfPlot_ExtendedFakes; // @asogaard
public:
  enum Category {
    ALL, IN_JETS, N_FAKETYPES
  };
  InDetPerfPlot_fakes(InDetPlotBase *pParent, const std::string &dirName);
  void fill(const xAOD::TrackParticle &trkprt, const bool isFake, const Category f = ALL);
  void fillLinkedandUnlinked(const xAOD::TrackParticle &trkprt, float Prim_w, float Sec_w, float Unlinked_w);
  void fillIncFakeDenom(const xAOD::TruthParticle &particle);
  void fillIncTrkRate(const unsigned int nMuEvents,std::vector<int> incTrkNum, std::vector<int> incTrkDenom);
private:
  ///fakes Histograms
  TH1 *m_fakepT;
  TH1 *m_fakePtLow;
  TH1 *m_fakephi;
  TH1 *m_fakeeta;
  TH1 *m_faked0;
  TH1 *m_fakez0;
  // fake rates as TProfiles
  TProfile *m_track_fakerate_vs_pt;
  TProfile *m_track_fakerate_vs_eta;
  TProfile *m_track_fakerate_vs_phi;
  TProfile *m_track_fakerate_vs_d0;
  TProfile *m_track_fakerate_vs_z0;
  TH1* m_incFakeNum_pt1;
  TH1* m_incFakeNum_pt2;
  TH1* m_incFakeNum_pt5;
  TProfile *m_fakeEtaTotal;
  TProfile *m_fakePtPrimary;
  TProfile *m_fakeetaPrimary;
  TProfile *m_fakePhiPrimary;
  TProfile *m_faked0Primary;
  TProfile *m_fakez0Primary;
  TProfile *m_fakePtSecondary;
  TProfile *m_fakeetaSecondary;
  TProfile *m_fakePhiSecondary;
  TProfile *m_faked0Secondary;
  TProfile *m_fakez0Secondary;
  TH1* m_incFakeDenomEta_pt1;
  TH1* m_incFakeDenomEta_pt2;
  TH1* m_incFakeDenomEta_pt5;
  TProfile *m_fakePtUnlinkedFrac;                                                       
  TProfile *m_fakeetaUnlinkedFrac;
  TProfile *m_fakePhiUnlinkedFrac;
  TProfile *m_faked0UnlinkedFrac;                                                        
  TProfile *m_fakez0UnlinkedFrac;
  TH1 *m_incFakeEta_pt1;
  TH1 *m_incFakeEta_pt2;
  TH1 *m_incFakeEta_pt5;
  TH1* m_nTracks_vs_mu;
  TH1* m_nTruth_vs_mu;
  TH1* m_incTrkRate_vs_mu;
  TH1* m_nTracks_vs_mu2;
  TH1* m_nTruth_vs_mu2;
  TH1* m_incTrkRate_vs_mu2;
  TH1* m_nTracks_vs_mu3;
  TH1* m_nTruth_vs_mu3;
  TH1* m_incTrkRate_vs_mu3;
  TH1* m_mu;

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
};




#endif
