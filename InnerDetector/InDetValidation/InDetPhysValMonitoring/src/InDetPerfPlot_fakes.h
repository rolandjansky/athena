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
  void fillLinked(const xAOD::TrackParticle &trkprt, const xAOD::TruthParticle &particle, double prob);
  void fillUnlinked(const xAOD::TrackParticle &trkprt, double prob);
  void fillIncFakeDenom(const xAOD::TruthParticle &particle);
  void fillIncFake(int nTracks, double ifr, int nSelected);
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
  TProfile *m_incFakevsTracks;
  TH2 *m_selectedTracks_vs_nTracks;

  TH1 *m_fakePtNumPrimary;
  TH1 *m_fakephiNumPrimary;
  TH1 *m_fakeetaNumPrimary;
  TH1 *m_faked0NumPrimary;
  TH1 *m_fakez0NumPrimary;
  TH1 *m_fakePtNumSecondary;
  TH1 *m_fakephiNumSecondary;
  TH1 *m_fakeetaNumSecondary;
  TH1 *m_faked0NumSecondary;
  TH1 *m_fakez0NumSecondary;
  TH1 *m_fakePtDenom;
  TH1 *m_fakephiDenom;
  TH1 *m_fakeetaDenom;
  TH1 *m_faked0Denom;
  TH1 *m_fakez0Denom;
  TH1 *m_fakePtUnlinked;
  TH1 *m_fakephiUnlinked;
  TH1 *m_fakeetaUnlinked;
  TH1 *m_faked0Unlinked;
  TH1 *m_fakez0Unlinked;

  TH1 *m_fakePtPrimary;
  TH1 *m_fakephiPrimary;
  TH1 *m_fakeetaPrimary;
  TH1 *m_faked0Primary;
  TH1 *m_fakez0Primary;
  TH1 *m_fakePtSecondary;
  TH1 *m_fakephiSecondary;
  TH1 *m_fakeetaSecondary;
  TH1 *m_faked0Secondary;
  TH1 *m_fakez0Secondary;
  TH1 *m_fakePtUnlinkedFrac;
  TH1 *m_fakephiUnlinkedFrac;
  TH1 *m_fakeetaUnlinkedFrac;
  TH1 *m_faked0UnlinkedFrac;
  TH1 *m_fakez0UnlinkedFrac;

  TH1 *m_incFakeDenomEta;
  TH1 *m_incFakeDenomPt;
  TH1 *m_incFakeEta;
  TH1 *m_incFakePt;

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
};




#endif
