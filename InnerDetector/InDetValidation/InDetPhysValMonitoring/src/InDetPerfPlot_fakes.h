/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_FAKES
#define INDETPHYSVALMONITORING_INDETPERFPLOT_FAKES
/**
 * @file InDetPerfPlot_fakes.h
 * @author shaun roe
 **/




// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
// std includes
#include <string>
class TH1;
class TProfile;


///class holding fake plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_fakes: public InDetPlotBase {
  friend class InDetPerfPlot_ExtendedFakes; // @asogaard
public:
  enum Category {
    ALL, IN_JETS, N_FAKETYPES
  };
  // inclusive track rates cuts
  // pt: one histo per cut 
  static const unsigned int m_nPtCuts=3;
  static const unsigned int m_nPtHists=m_nPtCuts;
  // mu: one histo per interval + total histo
  static const unsigned int m_nEtaCuts=2;
  // 2 cuts --> 3 intervals + 1 for total histogram:
  static const unsigned int m_nEtaHists=m_nEtaCuts+2;
  
  InDetPerfPlot_fakes(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& trkprt, const bool isFake, const Category f = ALL);
  void fillLinkedandUnlinked(const xAOD::TrackParticle& trkprt, float Prim_w, float Sec_w, float Unlinked_w, unsigned int mu);
  void fillIncTrkRateVsEtaPtcut(std::array<TH1*, m_nPtHists>& hists, double trkpt, double trketa);
  void fillIncTrkRateVsMuEtacut(std::array<TH1*, m_nEtaHists>& hists,  double trketa, unsigned int mu);
  void fillIncFakeDenom(const xAOD::TruthParticle& particle, unsigned int mu);
  void setPlotErrors(TH1* hrat, TH1* hnum, TH1* hdenom);
private:
  ///fakes Histograms
  TH1* m_fakepT;
  TH1* m_fakePtLow;
  TH1* m_fakephi;
  TH1* m_fakeeta;
  TH1* m_faked0;
  TH1* m_fakez0;
  // fake rates as TProfiles
  TProfile* m_track_fakerate_vs_pt;
  TProfile* m_track_fakerate_vs_eta;
  TProfile* m_track_fakerate_vs_phi;
  TProfile* m_track_fakerate_vs_d0;
  TProfile* m_track_fakerate_vs_z0;
  TEfficiency* m_fakeEtaTotal;
  TEfficiency* m_fakePtTotal;
  TEfficiency* m_fakePtPrimary;
  TEfficiency* m_fakeetaPrimary;
  TEfficiency* m_fakePhiPrimary;
  TEfficiency* m_faked0Primary;
  TEfficiency* m_fakez0Primary;
  TEfficiency* m_fakePtSecondary;
  TEfficiency* m_fakeetaSecondary;
  TEfficiency* m_fakePhiSecondary;
  TEfficiency* m_faked0Secondary;
  TEfficiency* m_fakez0Secondary;

  TEfficiency* m_fakePtUnlinkedFrac;
  TEfficiency* m_fakeetaUnlinkedFrac;
  TEfficiency* m_fakePhiUnlinkedFrac;
  TEfficiency* m_faked0UnlinkedFrac;
  TEfficiency* m_fakez0UnlinkedFrac;

  
  // inclusive track rates
  // ---> vs eta, pt>ptcut:
  // track pt cut values [GeV]
  std::array<float, m_nPtCuts> m_incTrkRatePtcutVals;
  // strings for histogram names (avoid float to string)
  std::array<std::string, m_nPtHists> m_incTrkRatePtcutSuff;
  // # accepted reco tracks vs eta, pt>ptcut
  std::array<TH1*, m_nPtHists> m_nRecTrkVsEtaPtcut;
  // # accepted truth tracks vs eta, pt>ptcut
  std::array<TH1*, m_nPtHists> m_nTruthTrkVsEtaPtcut;
  // (# acc reco)/(# acc truth) vs eta, pt>ptcut
  std::array<TH1*, m_nPtHists> m_incTrkRateVsEtaPtcut;
  // ---> vs mu, |eta|<cutlow && |eta|>cuthigh 
  std::array<float, m_nEtaCuts> m_incTrkRateEtacutVals;
  // strings for histogram names (avoid float to string)
  std::array<std::string, m_nEtaHists> m_incTrkRateEtacutSuff;
  // # accepted reco tracks vs eta, pt>ptcut
  std::array<TH1*, m_nEtaHists> m_nRecTrkVsMuEtacut;
  // # accepted truth tracks vs eta, pt>ptcut
  std::array<TH1*, m_nEtaHists> m_nTruthTrkVsMuEtacut;
  // (# acc reco)/(# acc truth) vs eta, pt>ptcut
  std::array<TH1*, m_nEtaHists> m_incTrkRateVsMuEtacut;

  TH1* m_mu;

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
};




#endif

