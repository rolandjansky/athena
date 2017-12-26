// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file JpsiAnalysis.cxx */
/** @author Mark Sutton (sutt@cern.ch) **/
/** @author Peter Wijeratne (paw@hep.ucl.ac.uk) **/
/** @author Christian Gutschow (chris.g@cern.ch) **/

#include "TrigInDetAnalysis/TIDAVertex.h"

#include "TrigIDJpsiMonitoring/JpsiAnalysis.h"
#include "TrigIDJpsiMonitoring/JpsiTrack.h"
#include "AthenaKernel/Units.h"

#include "TLorentzVector.h"

#include <stdexcept>

using std::fabs;
using std::sqrt;
using std::pow;
using std::string;
using std::cout;
using std::endl;
using std::vector;
using Athena::Units::GeV;

//#include "Particle/TrackParticle.h"


void JpsiAnalysis::initialise() { 

  //+++ pT ranges
  float tmp_absResPt = 0.0005;

  const int pTResBins = 100;

  //+++ Eta ranges
  float tmp_maxEta    = 3.5;
  float tmp_absResEta = 0.1; // 0.00025;
  float absResEta2D = 2.0;

  //+++ Phi ranges
  float tmp_maxPhi = 3.5;
  float tmp_absResPhi = 0.1; // 0.00025;
  float absResPhi2D = 3.2;

  float tmp_absResR = 0.1;
  const int rResBins = 500;

  const int etaBins    = 60;
  const int etaResBins = 800;
  const int etaResBins2D = 800;

  const int phiBins    = 36;
  const int phiResBins = 800;
  const int phiResBins2D = 640;

  const int   zBins = 100;      
  const float zMax  = 400;

  const int   zresBins = 110;      
  const float zresMax  = 10;

  const int   d0Bins = 1000;
  const float d0Max  = 10;

  const int   d0resBins = 110;      
  const float d0resMax  = 5;

  // beamspot corrected position

  const int   a0Bins = 1000;
  const float a0Max  = 10;

  const int   a0resBins = 110;      
  const float a0resMax  = 5;

  //+++ Book histograms

  // calculate a logarithmic binning in pt  
  /*  const int  ptnbins = 500;  
  float     ptbinlims[ptnbins+1];
  for ( int i=0 ; i<=ptnbins ; i++ ) {     ptbinlims[i] = pow(10, 2.0*i/ptnbins+2); }*/

  int pTbins = 50;
  float pTmax = 50.;
  float pTmin = 0.;
  int invmBins = 50;
  float invmMax = 5.;
  float invmMin = 0.;
  bool isZ = m_chainName.find("Z") != string::npos;
  isZ |= m_chainName.find("mu24i") != string::npos;
  isZ |= m_chainName.find("mu20")  != string::npos;
  if(isZ) {
    pTbins = 150;
    pTmax = 150.;
    invmBins = 400;
    invmMax = 110.;
    invmMin = 70.;
  }

  // COUNTERS
  addHistogram( new TH1F(  "trigger_after", "after trigger", 1,  0.0, 1.0 ) );
  addHistogram( new TH1F(  "cutflow",       "cutflow",       7, -0.5, 6.5 ) );
  addHistogram( new TH1F(  "cutflow_tight", "cutflow tight", 7, -0.5, 6.5 ) );

  // "reference" quantities
  addHistogram(  new TH1F(  "pT",   "pT",   pTbins,   -pTmin, pTmax ) );
  addHistogram(  new TH1F( "eta",  "eta",   etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram(  new TH1F( "phi",  "phi",   phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  addHistogram(  new TH1F(  "z0",   "z0",     zBins,        -zMax,       zMax ) );
  addHistogram(  new TH1F(  "d0",   "d0",    d0Bins,       -d0Max,      d0Max ) );
  addHistogram(  new TH1F(  "a0",   "a0",    a0Bins,       -a0Max,      a0Max ) );

  // "test" quantities
  addHistogram(    new TH1F(  "pT_rec",   "pT_rec",   pTbins, -pTmin, pTmax ) );
  addHistogram(    new TH1F( "eta_rec",  "eta_rec",   etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram(    new TH1F( "phi_rec",  "phi_rec",   phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  addHistogram(    new TH1F(  "z0_rec",   "z0_rec",     zBins,        -zMax,       zMax ) );
  addHistogram(    new TH1F(  "d0_rec",   "d0_rec",    d0Bins,       -d0Max,      d0Max ) );
  addHistogram(    new TH1F(  "a0_rec",   "a0_rec",    a0Bins,       -a0Max,      a0Max ) );

  // ID track quantities
  addHistogram( new TH1F(  "pT_track",  "pT_track",  pTbins,   -pTmin,      pTmax      ) );
  addHistogram( new TH1F( "eta_track", "eta_track",  etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram( new TH1F( "phi_track", "phi_track",  phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  // with back-to-back requirement (in dPhi between tag and probe)
  addHistogram( new TH1F(  "pT_track_b2b",  "pT_track",  pTbins,   -pTmin,      pTmax      ) );
  addHistogram( new TH1F( "eta_track_b2b", "eta_track",  etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram( new TH1F( "phi_track_b2b", "phi_track",  phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  // offline track quantities
  addHistogram( new TH1F(  "pT_offtrack",  "pT_track",  pTbins,   -pTmin,      pTmax      ) );
  addHistogram( new TH1F( "eta_offtrack", "eta_track",  etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram( new TH1F( "phi_offtrack", "phi_track",  phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  // offline track quantities (with pT/ET cut)
  addHistogram( new TH1F(  "pT_offtrackEt",  "pT_track",  pTbins,   -pTmin,      pTmax      ) );
  addHistogram( new TH1F( "eta_offtrackEt", "eta_track",  etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram( new TH1F( "phi_offtrackEt", "phi_track",  phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
  // offline track quantities (with brem cut)
  addHistogram( new TH1F(  "pT_offtrackbrem",  "pT_track",  pTbins,   -pTmin,      pTmax      ) );
  addHistogram( new TH1F( "eta_offtrackbrem", "eta_track",  etaBins,  -tmp_maxEta, tmp_maxEta ) );
  addHistogram( new TH1F( "phi_offtrackbrem", "phi_track",  phiBins,  -tmp_maxPhi, tmp_maxPhi ) );
    
  // resolutions
  addHistogram( new TH1F("pT_res",     "pT_res",  pTResBins,  -tmp_absResPt,  tmp_absResPt ) );
  addHistogram( new TH1F("eta_res",    "eta_res", etaResBins, -tmp_absResEta, tmp_absResEta) );
  addHistogram( new TH1F("phi_res",    "phi_res", phiResBins, -tmp_absResPhi, tmp_absResPhi) );
  addHistogram( new TH1F("z0_res",     "z0_res",  zresBins,   -zresMax,       zresMax      ) );
  addHistogram( new TH1F("d0_res",     "d0_res",  d0resBins,  -0.5*d0resMax,  0.5*d0resMax ) );
  addHistogram( new TH1F("a0_res",     "a0_res",  a0resBins,  -0.5*a0resMax,  0.5*a0resMax ) );
  addHistogram( new TH1F("etaOff_res", "etaOres", etaResBins, -tmp_absResEta, tmp_absResEta) );
  addHistogram( new TH1F("phiOff_res", "phiOres", phiResBins, -tmp_absResPhi, tmp_absResPhi) );
  addHistogram( new TH1F("rOff_res",   "rOff_res",rResBins,   0.0,            tmp_absResR  ) );
  addHistogram( new TH1F("minDr_res",  "rOff_res",rResBins,   0.0,            tmp_absResR  ) );

  /// delta-r between probe and its matched offline track
  addHistogram( new TH1F("probe_off_dr",   "probe_off_dr",    300, 0., 0.3) );
  addHistogram( new TH1F("probe_off_deta", "probe_off_deta",  etaResBins, -tmp_absResEta, tmp_absResEta) );
  addHistogram( new TH1F("probe_off_dphi", "probe_off_dphi",  phiResBins, -tmp_absResPhi, tmp_absResPhi) );

  addHistogram( new TH1F("dphi_tp", "dPhi between tag and probe", 200, -1., 1.) );

  addHistogram( new TH2F("asymEta", "asymEta", etaResBins2D, -absResEta2D, absResEta2D, etaResBins2D, -absResEta2D, absResEta2D ) );
  addHistogram( new TH2F("asymPhi", "asymPhi", phiResBins2D, -absResPhi2D, absResPhi2D, phiResBins2D, -absResPhi2D, absResPhi2D ) );

  // # tracks
  int Ntracks = 50;

  addHistogram( new TH1F( "ntracks",     "ntracks",     (Ntracks-40), -0.5, float(Ntracks-39.5) ) );
  addHistogram( new TH1F( "ntracks_rec", "ntracks_rec", Ntracks, -0.5, float(Ntracks+0.5) ) );

  //hit occupancies (muon system)
  addHistogram(  new TH1F( "nRpcHitsEta",  "nRpcHitsEta",   21,  -0.5, 20.5 ) );
  addHistogram(  new TH1F( "nTgcHitsEta",  "nTgcHitsEta",   21,  -0.5, 20.5 ) );
  addHistogram(  new TH1F( "nCscHitsEta",  "nCscHitsEta",   21,  -0.5, 20.5 ) );
  addHistogram(  new TH1F( "nRpcHitsPhi",  "nRpcHitsPhi",   21,  -0.5, 20.5 ) );
  addHistogram(  new TH1F( "nTgcHitsPhi",  "nTgcHitsPhi",   21,  -0.5, 20.5 ) );
  addHistogram(  new TH1F( "nCscHitsPhi",  "nCscHitsPhi",   21,  -0.5, 20.5 ) );
  addHistogram(  new TH1F( "nMdtHits",  "nMdtHits",   51,  -0.5, 50.5 ) );

  addHistogram(  new TH1F("tagMatch_invmass","All tag&probe candidates;Invariant mass [GeV]", invmBins, invmMin, invmMax) );
  addHistogram(  new TH1F("tagAndProbe_invmass","All tag&probe candidates;Invariant mass [GeV]", invmBins, invmMin, invmMax) );
  addHistogram(  new TH1F("tagAndProbe_invmass_wo_match","Tag&probe candidates wo match;Invariant mass [GeV]", invmBins, invmMin, invmMax) );
  addHistogram(  new TH1F("tagAndProbe_invmass_offline","Tag&probe candidates (with offline electron);Invariant mass [GeV]", invmBins, invmMin, invmMax) );
  addHistogram(  new TH1F(  "Zpt",   "dilepton pt",   pTbins, pTmin, pTmax ) );
  addHistogram(  new TH1F(  "pTmiss",   "pTmiss",   pTbins, pTmin, pTmax ) );
  m_pT_eff = new TProfile("pT_eff", "Efficiency versus pT; pT [MeV]", pTbins, pTmin, pTmax, 0., 110.);
  m_pT_eff_d0cut = new TProfile("pT_eff_d0cut", "Efficiency versus pT; pT [MeV]", pTbins, pTmin, pTmax, 0., 110.);    
  m_invmass_eff = new TProfile("invmass_eff", "Efficiency versus Invariant Mass; Invariant Mass [GeV]", invmBins, invmMin, invmMax, 0., 110.);
  m_invmass_eff_d0cut = new TProfile("invmass_eff_d0cut", "Efficiency versus Invariant Mass; Invariant Mass [GeV]", invmBins, invmMin, invmMax, 0., 110.);
  m_pT_eff_offline = new TProfile("pT_eff_offline", "Efficiency versus pT; pT [MeV]", pTbins, pTmin, pTmax, 0., 110.);
  m_pT_eff_d0cut_offline = new TProfile("pT_eff_d0cut_offline", "Efficiency versus pT; pT [MeV]", pTbins, pTmin, pTmax, 0., 110.);    
  m_invmass_eff_offline = new TProfile("invmass_eff_offline", "Efficiency versus Invariant Mass; Invariant Mass [GeV]", invmBins, invmMin, invmMax, 0., 110.);
  m_invmass_eff_d0cut_offline = new TProfile("invmass_eff_d0cut_offline", "Efficiency versus Invariant Mass; Invariant Mass [GeV]", invmBins, invmMin, invmMax, 0., 110.);

  m_eta_eff = new TProfile("eta_eff", "Efficiency versus #eta; #eta", 32, -4., 4., 0., 110.);
  m_vert_eff = new TProfile("vert_eff", "Efficiency versus nVertices; nvertices", 36, -0.5, 35.5, 0., 110.);
  m_z0_eff = new TProfile("z0_eff", "Efficiency versus z0; z0[mm]", 20, 0., 200., 0., 110.);
  m_d0_eff = new TProfile("d0_eff", "Efficiency versus d0; d0[mm]", d0Bins, -d0Max, d0Max, 0., 110.);
  m_phi_eff = new TProfile("phi_eff", "Efficiency versus #phi; #phi[rads]", 20., -4., 4., 0., 110.);
  m_track_eff = new TProfile("track_eff", "Efficiency versus nTracks; ntracks", 80., 0., 800., 0., 110.);

  m_eta_eff_d0cut = new TProfile("eta_eff_d0cut", "Efficiency versus #eta; #eta", 32, -4., 4., 0., 110.);
  m_vert_eff_d0cut = new TProfile("vert_eff_d0cut", "Efficiency versus nVertices; nvertices", 36, -0.5, 35.5, 0., 110.);
  m_z0_eff_d0cut = new TProfile("z0_eff_d0cut", "Efficiency versus z0; z0[mm]", 20, 0., 200., 0., 110.);
  m_d0_eff_d0cut = new TProfile("d0_eff_d0cut", "Efficiency versus d0; d0[mm]", d0Bins, -d0Max, d0Max, 0., 110.);
  m_phi_eff_d0cut = new TProfile("phi_eff_d0cut", "Efficiency versus #phi; #phi[rads]", 20., -4., 4., 0., 110.);
  m_track_eff_d0cut = new TProfile("track_eff_d0cut", "Efficiency versus nTracks; ntracks", 80., 0., 800., 0., 110.);

  m_eta_eff_offline = new TProfile("eta_eff_offline", "Efficiency versus #eta; #eta", 32, -4., 4., 0., 110.);
  m_vert_eff_offline = new TProfile("vert_eff_offline", "Efficiency versus nVertices; nvertices", 36, -0.5, 35.5, 0., 110.);
  m_z0_eff_offline = new TProfile("z0_eff_offline", "Efficiency versus z0; z0[mm]", 20, 0., 200., 0., 110.);
  m_d0_eff_offline = new TProfile("d0_eff_offline", "Efficiency versus d0; d0[mm]", d0Bins, -d0Max, d0Max, 0., 110.);
  m_phi_eff_offline = new TProfile("phi_eff_offline", "Efficiency versus #phi; #phi[rads]", 20., -4., 4., 0., 110.);
  m_track_eff_offline = new TProfile("track_eff_offline", "Efficiency versus nTracks; ntracks", 80., 0., 800., 0., 110.);

  m_eta_eff_d0cut_offline = new TProfile("eta_eff_d0cut_offline", "Efficiency versus #eta; #eta", 32, -4., 4., 0., 110.);
  m_vert_eff_d0cut_offline = new TProfile("vert_eff_d0cut_offline", "Efficiency versus nVertices; nvertices", 36, -0.5, 35.5, 0., 110.);
  m_z0_eff_d0cut_offline = new TProfile("z0_eff_d0cut_offline", "Efficiency versus z0; z0[mm]", 20, 0., 200., 0., 110.);
  m_d0_eff_d0cut_offline = new TProfile("d0_eff_d0cut_offline", "Efficiency versus d0; d0[mm]", d0Bins, -d0Max, d0Max, 0., 110.);
  m_phi_eff_d0cut_offline = new TProfile("phi_eff_d0cut_offline", "Efficiency versus #phi; #phi[rads]", 20., -4., 4., 0., 110.);
  m_track_eff_d0cut_offline = new TProfile("track_eff_d0cut_offline", "Efficiency versus nTracks; ntracks", 40., 0., 400., 0., 110.);

  /*  m_pT_pur = new TProfile("pT_pur", "Purity versus pT; pT [MeV]", 100, 0., 100000., 0., 110.);
  m_eta_pur = new TProfile("eta_pur", "Purity versus #eta; #eta", 20, -4., 4., 0., 110.);
  m_z0_pur = new TProfile("z0_pur", "Purity versus z0; z0[mm]", 20, 0., 200., 0., 110.);
  m_d0_pur = new TProfile("d0_pur", "Purity versus d0; d0[mm]", 20, 0., 50., 0., 110.);
  m_phi_pur = new TProfile("phi_pur", "Purity versus #phi; #phi[rads]", 20., -4., 4., 0., 110.);*/

  /*  addHistogram( m_pT_pur );
  addHistogram( m_eta_pur );
  addHistogram( m_z0_pur );
  addHistogram( m_d0_pur);
  addHistogram( m_phi_pur );*/

  addHistogram( m_pT_eff );
  addHistogram( m_eta_eff );
  addHistogram( m_vert_eff );
  addHistogram( m_z0_eff );
  addHistogram( m_d0_eff);
  addHistogram( m_phi_eff );
  addHistogram( m_track_eff );
  addHistogram( m_invmass_eff );

  /// |d0| > 1mm cut, to study delayed decays
  addHistogram( m_pT_eff_d0cut );
  addHistogram( m_eta_eff_d0cut );
  addHistogram( m_vert_eff_d0cut );
  addHistogram( m_z0_eff_d0cut );
  addHistogram( m_d0_eff_d0cut);
  addHistogram( m_phi_eff_d0cut );
  addHistogram( m_track_eff_d0cut );
  addHistogram( m_invmass_eff_d0cut );

  addHistogram( m_pT_eff_offline );
  addHistogram( m_eta_eff_offline );
  addHistogram( m_vert_eff_offline );
  addHistogram( m_z0_eff_offline );
  addHistogram( m_d0_eff_offline);
  addHistogram( m_phi_eff_offline );
  addHistogram( m_track_eff_offline );
  addHistogram( m_invmass_eff_offline );

  /// |d0| > 1mm cut, to study delayed decays
  addHistogram( m_pT_eff_d0cut_offline );
  addHistogram( m_eta_eff_d0cut_offline );
  addHistogram( m_vert_eff_d0cut_offline );
  addHistogram( m_z0_eff_d0cut_offline );
  addHistogram( m_d0_eff_d0cut_offline);
  addHistogram( m_phi_eff_d0cut_offline );
  addHistogram( m_track_eff_d0cut_offline );
  addHistogram( m_invmass_eff_d0cut_offline );

  ///
  addHistogram( new TH1F("tag_offline_invmass", "tag_offline_invmass", invmBins, invmMin, invmMax) );
  addHistogram( new TH2F("tomass_vs_offpt", "tomass_vs_offpt", pTbins, pTmin, pTmax, 50, invmMin, invmMax) );

  /// towards asymmetric errors...

  const int nTrksBins   = 250;
  const float nTrksMax = 1000;
  const int nVtxsBins   = 41;
  const float nVtxsMax = 40.5;
  const int muBins      = 1000;
  const float muMax    = 100.5;

  //standard plots
  addHistogram( new TH1F("invmass_tt","invmass_tt", 500, 0., 500.) );
  addHistogram( new TH1F("invmassnum","invmassnum",invmBins, invmMin, invmMax) );
  addHistogram( new TH1F("invmassden","invmassden",invmBins, invmMin, invmMax) );
  addHistogram( new TH1F("ETnum",    "ETnum", 200, 0., 1.) );
  addHistogram( new TH1F("ETden",    "ETden", 200, 0., 1.) );
  addHistogram( new TH1F("pTnum",    "pTnum", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTden",    "pTden", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTEtnum",  "pTnum", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTEtden",  "pTden", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTbremnum",  "pTnum", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTbremden",  "pTden", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTb2bnum", "pTnum", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTb2bden", "pTden", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTb2bloosenum", "pTnum", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTb2blooseden", "pTden", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("d0num", "d0num", d0Bins, -d0Max, d0Max ) );
  addHistogram( new TH1F("d0den", "d0den", d0Bins, -d0Max, d0Max ) );
  addHistogram( new TH1F("z0num", "z0num", zBins, -zMax, zMax) );
  addHistogram( new TH1F("z0den", "z0den", zBins, -zMax, zMax) );
  addHistogram( new TH1F("absd0num", "absd0num", d0Bins/2, 0, d0Max ) );
  addHistogram( new TH1F("absd0den", "absd0den", d0Bins/2, 0, d0Max ) );
  addHistogram( new TH1F("absz0num", "absz0num", zBins/2, 0, zMax) );
  addHistogram( new TH1F("absz0den", "absz0den", zBins/2, 0, zMax) );
  addHistogram( new TH1F("phinum", "phinum", 20., -4., 4.) );
  addHistogram( new TH1F("phiden", "phiden", 20., -4., 4.) );
  addHistogram( new TH1F("etanum", "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etaden", "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etaEtnum",  "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etaEtden",  "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etabremnum",  "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etabremden",  "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etab2bnum", "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etab2bden", "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etab2bloosenum", "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etab2blooseden", "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("vertnum", "vertnum", nVtxsBins, -0.5, nVtxsMax) );
  addHistogram( new TH1F("vertden", "vertden", nVtxsBins, -0.5, nVtxsMax) );
  addHistogram( new TH1F("tracknum", "tracknum", nTrksBins, 0., nTrksMax) );
  addHistogram( new TH1F("trackden", "trackden", nTrksBins, 0., nTrksMax) );
  addHistogram( new TH1F("munum", "munum", muBins, -0.5, muMax) );
  addHistogram( new TH1F("muden", "muden", muBins, -0.5, muMax) );
  addHistogram( new TH1F("mub2bnum", "munum", muBins, -0.5, muMax) );
  addHistogram( new TH1F("mub2bden", "muden", muBins, -0.5, muMax) );
  addHistogram( new TH1F("mub2bloosenum", "munum", muBins, -0.5, muMax) );
  addHistogram( new TH1F("mub2blooseden", "muden", muBins, -0.5, muMax) );
  addHistogram( new TH1F("drnum", "drnum", 100., 0., 1.0) );
  addHistogram( new TH1F("drden", "drden", 100., 0., 1.0) );

  //plots where an offline track is required to be matched to the probe
  addHistogram( new TH1F("invmassnum_off","invmassnum_off",invmBins, invmMin, invmMax) );
  addHistogram( new TH1F("invmassden_off","invmassden_off",invmBins, invmMin, invmMax) );
  addHistogram( new TH1F("ETnum_off",    "ETnum_off", 200, 0., 1.) );
  addHistogram( new TH1F("ETden_off",    "ETden_off", 200, 0., 1.) );
  addHistogram( new TH1F("pTnum_off",    "pTnum_off", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTden_off",    "pTden_off", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTEtnum_off",  "pTnum",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTEtden_off",  "pTden",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTbremnum_off",  "pTnum",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTbremden_off",  "pTden",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTofftracknum_off",    "pTnum_off", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTofftrackden_off",    "pTden_off", pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTofftrackEtnum_off",  "pTnum",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTofftrackEtden_off",  "pTden",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTofftrackbremnum_off",  "pTnum",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTofftrackbremden_off",  "pTden",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTb2bnum_off", "pTnum",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTb2bden_off", "pTden",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTb2bloosenum_off", "pTnum",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pTb2blooseden_off", "pTden",     pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("d0num_off", "d0num_off", d0Bins, -d0Max, d0Max ) );
  addHistogram( new TH1F("d0den_off", "d0den_off", d0Bins, -d0Max, d0Max ) );
  addHistogram( new TH1F("z0num_off", "z0num_off", zBins, -zMax, zMax) );
  addHistogram( new TH1F("z0den_off", "z0den_off", zBins, -zMax, zMax ) );
  addHistogram( new TH1F("absd0num_off", "absd0num_off", d0Bins/2, 0, d0Max ) );
  addHistogram( new TH1F("absd0den_off", "absd0den_off", d0Bins/2, 0, d0Max ) );
  addHistogram( new TH1F("absz0num_off", "absz0num_off", zBins/2, 0, zMax) );
  addHistogram( new TH1F("absz0den_off", "absz0den_off", zBins/2, 0, zMax) );
  addHistogram( new TH1F("etanum_off", "etanum_off", 32, -4., 4.) );
  addHistogram( new TH1F("etaden_off", "etaden_off", 32, -4., 4.) );
  addHistogram( new TH1F("etaEtnum_off",  "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etaEtden_off",  "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etabremnum_off",  "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etabremden_off",  "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etaofftracknum_off", "etanum_off", 32, -4., 4.) );
  addHistogram( new TH1F("etaofftrackden_off", "etaden_off", 32, -4., 4.) );
  addHistogram( new TH1F("etaofftrackEtnum_off",  "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etaofftrackEtden_off",  "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etaofftrackbremnum_off",  "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etaofftrackbremden_off",  "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etab2bnum_off", "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etab2bden_off", "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("etab2bloosenum_off", "etanum", 32, -4., 4.) );
  addHistogram( new TH1F("etab2blooseden_off", "etaden", 32, -4., 4.) );
  addHistogram( new TH1F("phinum_off", "phinum_off", 20., -4., 4.) );
  addHistogram( new TH1F("phiden_off", "phiden_off", 20., -4., 4.) );
  addHistogram( new TH1F("vertnum_off", "vertnum_off", nVtxsBins, -0.5, nVtxsMax) );
  addHistogram( new TH1F("vertden_off", "vertden_off", nVtxsBins, -0.5, nVtxsMax) );
  addHistogram( new TH1F("tracknum_off", "tracknum_off", nTrksBins, 0., nTrksMax) );
  addHistogram( new TH1F("trackden_off", "trackden_off", nTrksBins, 0., nTrksMax) );
  addHistogram( new TH1F("munum_off", "munum_off", muBins, -0.5, muMax) );
  addHistogram( new TH1F("muden_off", "muden_off", muBins, -0.5, muMax) );
  addHistogram( new TH1F("mub2bnum_off", "munum_off", muBins, -0.5, muMax) );
  addHistogram( new TH1F("mub2bden_off", "muden_off", muBins, -0.5, muMax) );
  addHistogram( new TH1F("mub2bloosenum_off", "munum_off", muBins, -0.5, muMax) );
  addHistogram( new TH1F("mub2blooseden_off", "muden_off", muBins, -0.5, muMax) );
  addHistogram( new TH1F("drnum_off", "drnum_off", 100., 0., 1.0) );
  addHistogram( new TH1F("drden_off", "drden_off", 100., 0., 1.0) );

  //plots where the offline track pT is used instead of the probe pT
  addHistogram( new TH1F("pToffnum_off", "pToffnum_off",   pTbins, pTmin, pTmax ) );
  addHistogram( new TH1F("pToffden_off", "pToffden_off",   pTbins, pTmin, pTmax ) );

  //plot of delta-r between tag and probe 
  addHistogram( new TH1F("deltarnum", "deltarnum",   400, 0., 5. ) );
  addHistogram( new TH1F("deltarden", "deltarden",   400, 0., 5. ) );
  addHistogram( new TH1F("deltarnum_lowpt", "deltarnum_lowpt",   400, 0., 5. ) );
  addHistogram( new TH1F("deltarden_lowpt", "deltarden_lowpt",   400, 0., 5. ) );

  /// beamspot
  addHistogram( new TH2F("beamspotxy", "beamspotxy", 50, -2., 2., 50, -2., 2.) );

  ///tmp
  if(m_chainName.find("Zmumu") != string::npos) {
    invmMin = 0.;
  }
  addHistogram( new TH2F("invmass_vs_tinvmass", "invmass_vs_tinvmass", invmBins, invmMin, invmMax, invmBins, invmMin, invmMax) );

  addHistogram( new TH1F("pToffnum_d0cut", "pToffnum_d0cut", pTbins, pTmin, pTmax) );
  addHistogram( new TH1F("pToffden_d0cut", "pToffden_d0cut", pTbins, pTmin, pTmax) );
  addHistogram( new TH1F("pTnum_d0cut",    "pTnum_d0cut",    pTbins, pTmin, pTmax) );
  addHistogram( new TH1F("pTden_d0cut",    "pTden_d0cut",    pTbins, pTmin, pTmax) );

  ///tmp
  addHistogram( new TH2F("invmass_vs_deltar", "invmass_vs_deltar", 400,      0.,      5.,    invmBins, invmMin, invmMax) );
  addHistogram( new TH2F("pt_vs_deltar",      "pt_vs_deltar",      400,      0.,      5.,    pTbins,   pTmin,   pTmax  ) );
  addHistogram( new TH2F("pt_vs_offpt",       "pt_vs_offpt",       pTbins,   pTmin,   pTmax, pTbins,   pTmin,   pTmax  ) );
  addHistogram( new TH1F("tinvmass_miss",     "tinvmass_miss",     invmBins, invmMin, invmMax ) );

}
  
void JpsiAnalysis::finalise() { }

/*template<class T>
bool fillMap(std::map<std::string, T*> &m_histos, const std::string &name, float w) {
  std::map<string, T*>::const_iterator hmitr = m_histos.find(name.c_string());
  if(hmitr != m_histos.end()) {
    if(hmitr->second) { hmitr->second->Fill( w ); return true; }
  
  }
  return false;
}*/


void JpsiAnalysis::execute(const vector<TIDA::Track*>& probeTracks,
                           const vector<TIDA::Track*>& testtracks,
                   	       TrackAssociator* matcher=0) {

  m_speaker->msg(MSG::DEBUG) << "Start of JpsiAnalysis execute ..." << endmsg;
  matcher = 0;
  if(!matcher)  m_speaker->msg(MSG::DEBUG) << "Matcher not used." << endmsg;

  // should have these as a class variable   
  static string varName[13] = { "pT", "eta", "phi", "z0", "d0", "a0", 
                                "nRpcHitsEta", "nTgcHitsEta", "nCscHitsEta", 
                                "nRpcHitsPhi", "nTgcHitsPhi", "nCscHitsPhi", "nMdtHits" };  

  std::map<string, TH1F*>::iterator hmitr = m_histos.find("ntracks");
  if(hmitr != m_histos.end())  hmitr->second->Fill(probeTracks.size());
  //fillMap(m_histos, "ntracks", probeTracks.size());
 
  hmitr = m_histos.find("ntracks_rec");
  if(hmitr != m_histos.end())  hmitr->second->Fill(testtracks.size());

  hmitr = m_histos.find("trigger_after");
  if(hmitr != m_histos.end())  hmitr->second->Fill( 0.5 );

  hmitr = m_histos.find("cutflow"); // trigger hits
  if(hmitr != m_histos.end())  hmitr->second->Fill( 0.0 );
  hmitr = m_histos.find("cutflow_tight"); // trigger hits
  if(hmitr != m_histos.end())  hmitr->second->Fill( 0.0 );

  std::map<string, TH2F*>::iterator hmitr2d;

  float pTt     = 0.0, z0t = 0.0, etat = 0.0,
        phit    = 0.0, d0t = 0.0, a0t  = 0.0,
        invmass = 0.0;
  int nvert = 0, ntracks = 0; //, q = 0;

  ///default is J/Psi
  float mass = 3.097*GeV;
  m_plusMass  = 2.903*GeV;
  m_minusMass = 2.097*GeV;
  if(m_chainName.find("Z") != string::npos) { 
    mass = 90.097*GeV;
    m_plusMass  = 19.903*GeV;
    m_minusMass = 20.097*GeV;   
  }

  /// beamspot x,y
  //hmitr2d = m_histos2d.find("beamspotxy");
  //if(hmitr2d != m_histos2d.end())  hmitr2d->second->Fill(m_yBeamReference, m_xBeamReference, 1.0);

  /*
   *
   * THIS IS WHERE SOME SERIOUS BUSINESS IS HAPPENING...
   *
   *
   */
  bool cutflow_check[] = { 1, 1, 1, 1 };
  bool cutflow_check_tight[] = { 1, 1, 1, 1 };
  for(unsigned int i=0; i < probeTracks.size(); i++) {

    TIDA::JpsiTrack* probe = 0;
    const Wrapper::MuonTrack *tag = 0;
    if(probeTracks[i]) {
      probe = dynamic_cast<TIDA::JpsiTrack*> (probeTracks[i]);
      tag   = probe->getTag();
    }
    else throw std::runtime_error("*** TrigJpsiMonTool *** Probe not casting, aborting!!! *** TrigJpsiMonTool ***");

    m_speaker->msg(MSG::DEBUG) << "probe [ pT = " << probe->pT()  << " eta = " << probe->eta()
                               << " phi = "       << probe->phi() << " ]" << endmsg;
    m_speaker->msg(MSG::DEBUG) << "tag [ pT = "   << tag->pt()    << " eta = " << tag->eta()
                               << " phi = "       << tag->phi()   << " ]" << endmsg;
    cout << "probe [ pT = " << probe->pT()  << " eta = " << probe->eta()
         << " phi = "       << probe->phi() << " ]" << endl;
    cout << "tag [ pT = "   << tag->pt()    << " eta = " << tag->eta()
         << " phi = "       << tag->phi()   << " ]" << endl;

    int probeRoiId = probe->getRoiNum();
    int tagRoiId   = tag->roiNum();
    if(probeRoiId == tagRoiId) {
      m_speaker->msg(MSG::WARNING) << "*** TrigJpsiMonTool *** probe RoI == tag RoI!!! *** TrigJpsiMonTool ***" << endmsg;
      throw std::runtime_error("*** TrigJpsiMonTool *** probe RoI == tag RoI!!! *** TrigJpsiMonTool ***");
    }
    else m_speaker->msg(MSG::DEBUG) << "JPsiAnalysis -- probe ROI: " << probeRoiId << "  and tag ROI: " << tagRoiId << endmsg;
    
    m_speaker->msg(MSG::DEBUG) << "test tracks size: " << testtracks.size()
                               << " roi size: " << m_trackRois->size() << endmsg;
    cout << "test tracks size: " << testtracks.size() << endl;
    //for(unsigned int jj = 0; jj < testtracks.size(); ++jj)  cout << "testtracks - " << jj << " - " << testtracks[jj] << endl;
    //for( std::map<int,int>::iterator ii = m_trackRois->begin(); ii != m_trackRois->end(); ++ii) {
    //  cout << (*ii).first << " : " << (*ii).second << endl;
    //}

    float m = probe->mass();
    bool isTightPair = m > (mass-m_minusMass) && m < (mass+m_plusMass);
    //cout << "cutflow check ..." << endl;
    if(cutflow_check[0]) {
      cutflow_check[0] = 0;
      hmitr = m_histos.find("cutflow"); // got t&p candidate
      if(hmitr != m_histos.end())  hmitr->second->Fill( 1.0 );
    }
    //cout << "tight cutflow check ..." << endl;
    if(isTightPair && cutflow_check_tight[0]) {
      cutflow_check_tight[0] = 0;
      hmitr = m_histos.find("cutflow_tight"); // got t&p candidate
      if(hmitr != m_histos.end())  hmitr->second->Fill( 1.0 );
    }
    
    float mu_inTime = 0; //, mu_outOfTime = 0;
    
    m_speaker->msg(MSG::DEBUG) << "get event info ..." << endmsg;
    if(m_speaker->evtStore()->retrieve(m_eventInfo).isFailure())  m_speaker->msg(MSG::DEBUG) << "Failed to get EventInfo " << endmsg;
    else {
      mu_inTime    = m_eventInfo->actualInteractionsPerCrossing();
      //mu_outOfTime = m_eventInfo->averageInteractionsPerCrossing();
    }

    unsigned int runNumber   = m_eventInfo->event_ID()->run_number();
    unsigned int eventNumber = m_eventInfo->event_ID()->event_number();

    ///temp
    float offd0  = 0., offphi0 = 0., offz0 = 0., offpT = 0.,
          offeta = 0., offphi = 0., temp_Deta = 0., temp_Dphi = 0.,
          matched_brem = 0.;
    /// what if there are more ef than offline tracks?!?!
    //cout << "cutflow stuff ..." << endl;
    bool fillOffline = false;
    hmitr = m_histos.find("cutflow"); // offline matches
    if(cutflow_check[1] && hmitr != m_histos.end()) {
      //std::cout << "tag and probe pointers: " << m_offlineTags << "  " << m_offlineProbes << std::endl; 
      //std::cout << "i: " << i << " offlineTags: " << m_offlineTags->size() << " offlineProbes: " << m_offlineProbes->size() << std::endl;
      cutflow_check[1] = 0;
      if(m_offlineTags->at(i))    hmitr->second->Fill( 2.0 );
      if(m_offlineProbes->at(i))  hmitr->second->Fill( 3.0 );
      if(m_offlineProbes->at(i) && m_offlineTags->at(i))  hmitr->second->Fill( 4.0 );
    }
    hmitr = m_histos.find("cutflow_tight"); // offline matches
    if(isTightPair && cutflow_check_tight[1] && hmitr != m_histos.end()) {
      cutflow_check_tight[1] = 0;
      if(m_offlineTags->at(i))    hmitr->second->Fill( 2.0 );
      if(m_offlineProbes->at(i))  hmitr->second->Fill( 3.0 );
      if(m_offlineProbes->at(i) && m_offlineTags->at(i))  hmitr->second->Fill( 4.0 );
    }

    m_speaker->msg(MSG::DEBUG) << "require offline matches ..." << endmsg;

    bool matchOffline = m_offlineProbes->at(i) ? true : false;
    m_speaker->msg(MSG::DEBUG) << "matchOffline probes: " << matchOffline << endmsg;
    if(!m_isMuon) {
      matchOffline  = m_offlineTags->at(i) && m_tOffCharge->at(i) * m_pOffCharge->at(i) < 0.0;
      m_speaker->msg(MSG::DEBUG) << "offTag: " << m_offlineTags->at(i) << endmsg;
      m_speaker->msg(MSG::DEBUG) << "tOffCharge: " << m_tOffCharge->at(i) << endmsg;
      m_speaker->msg(MSG::DEBUG) << "pOffCharge: " << m_pOffCharge->at(i) << endmsg;
      m_speaker->msg(MSG::DEBUG) << "matchOffline tag: " << matchOffline << endmsg;
    }

    if(matchOffline) {

      // #ifndef TIDA_NEWTRACKING_H
      //      const Trk::MeasuredPerigee* measPer = m_offlineProbes->at(i)->measuredPerigee();
      // #else
      const Trk::Perigee* measPer = m_offlineProbes->at(i)->measuredPerigee();
      // #endif
      //      cout << "offd0 offset x = " << m_xBeamReference << " y = " << m_yBeamReference << endl;
      offd0   = measPer->parameters()[Trk::d0] + sin(m_offlineProbes->at(i)->phi())*m_xBeamReference;
      offd0  -= cos(m_offlineProbes->at(i)->phi())*m_yBeamReference;
      offphi0 = fixPhi( measPer->parameters()[Trk::phi0]);
      offz0   = measPer->parameters()[Trk::z0];
      offpT   = m_offlineProbes->at(i)->pt();
      offeta  = m_offlineProbes->at(i)->eta();
      offphi  = fixPhi(m_offlineProbes->at(i)->phi());
      temp_Deta = m_pOffDeta->at(i);
      temp_Dphi = fixPhi(m_pOffDphi->at(i));
      float et = m_pOffEt->at(i);
      if(et)  matched_brem = fabs(offpT) / et;

      hmitr = m_histos.find("probe_off_deta");
      if(hmitr!=m_histos.end())  hmitr->second->Fill(temp_Deta);
      hmitr = m_histos.find("probe_off_dphi");
      if(hmitr!=m_histos.end())  hmitr->second->Fill(temp_Dphi);
      hmitr = m_histos.find("probe_off_dr");
      if(hmitr!=m_histos.end())  hmitr->second->Fill(m_pOffDr->at(i));

      ///work out the tag-offline invmass
      TLorentzVector tag4vec, probe4vec, offTag4vec, offProbe4vec, comb4vec;
      float offtheta = 2. * atan(exp(-offeta));
      float pxoff    = offpT * cos(m_offlineProbes->at(i)->phi());
      float pyoff    = offpT * sin(m_offlineProbes->at(i)->phi());
      float pzoff    = offpT * cos(offtheta) / sin(offtheta);
      
      float Mass = m_isMuon? 105.66 : 0.511;

      tag4vec.SetPtEtaPhiE(tag->pt(), tag->eta(), fixPhi(tag->phi()),
                             sqrt(pow(Mass,2)+pow(tag->px(),2)+pow(tag->py(),2)+pow(tag->pz(),2)));
      probe4vec.SetPtEtaPhiE(probe->pT(), probe->eta(), fixPhi(probe->phi()),
                             sqrt(pow(Mass,2)+pow(probe->px(),2)+pow(probe->py(),2)+pow(probe->pz(),2)));
      offProbe4vec.SetPtEtaPhiE(offpT, offeta, offphi0,
                             sqrt(pow(Mass,2)+pow(pxoff,2)+pow(pyoff,2)+pow(pzoff,2)));
      comb4vec = tag4vec + offProbe4vec;
      float vmag = comb4vec.Mag()/GeV;
      // probe mass confirmation cut
      fillOffline  = !(vmag < 0.5);
      m_speaker->msg(MSG::DEBUG) << "vmag: " << vmag << endmsg;

      cout << "> HIGH PT INEFFICIENCY DEBUG:" << endl;
      cout << "> probe pT: " << probe->pT() << endl;
      cout << "> offline probe track information:" << endl;
      cout << "> offline probe track eta:" << offeta << endl;
      cout << "> offline probe track phi:" << offphi << endl;
      cout << "> offline probe track pT:" << offpT << endl;
      //cout << (*m_offlineProbes->at(i)) << endl;
      if(m_offlineTags->at(i)) {
        cout << "> offline tag track information:" << endl;
        float offTagPhi0  = fixPhi(((m_offlineTags->at(i)->measuredPerigee())->parameters())[Trk::phi0]);
        float offTagPt    = m_offlineTags->at(i)->pt();
        float offTagEta   = m_offlineTags->at(i)->eta();
        float offTagTheta = 2. * atan(exp(-offTagEta));
        float pxTag       = offTagPt * cos(m_offlineTags->at(i)->phi());
        float pyTag       = offTagPt * sin(m_offlineTags->at(i)->phi());
        float pzTag       = offTagPt * cos(offTagTheta) / sin(offTagTheta);
        cout << "> offline tag track eta:" << offTagEta << endl;
        cout << "> offline tag track phi:" << offTagPhi0 << endl;
        cout << "> offline tag track pT:" << offTagPt << endl;
        //cout << (*m_offlineTags->at(i)) << endl;
        offTag4vec.SetPtEtaPhiE(offTagPt, offTagEta, offTagPhi0,
                               sqrt(pow(Mass,2)+pow(pxTag,2)+pow(pyTag,2)+pow(pzTag,2)));
        hmitr = m_histos.find("Zpt");
        if(hmitr != m_histos.end())  hmitr->second->Fill((offProbe4vec + offTag4vec).Perp()/GeV);
      }

      hmitr = m_histos.find("tag_offline_invmass");
      if(hmitr != m_histos.end())  hmitr->second->Fill(probe->mass()/GeV);
      hmitr2d = m_histos2d.find("tomass_vs_offpt");
      if(hmitr2d != m_histos2d.end()) hmitr2d->second->Fill(fabs(offpT/GeV), vmag, 1.0);

      if(!m_isMuon) {
        float offTagPhi0  = fixPhi(((m_offlineTags->at(i)->measuredPerigee())->parameters())[Trk::phi0]);
        float offTagPt    = m_offlineTags->at(i)->pt();
        float offTagEta   = m_offlineTags->at(i)->eta();
        float offTagTheta = 2. * atan(exp(-offTagEta));
        float pxTag       = offTagPt * cos(m_offlineTags->at(i)->phi());
        float pyTag       = offTagPt * sin(m_offlineTags->at(i)->phi());
        float pzTag       = offTagPt * cos(offTagTheta) / sin(offTagTheta);
        offTag4vec.SetPtEtaPhiE(offTagPt, offTagEta, offTagPhi0,
                               sqrt(pow(Mass,2)+pow(pxTag,2)+pow(pyTag,2)+pow(pzTag,2)));
        // tag mass confirmation cut
        comb4vec = probe4vec + offTag4vec;
        vmag = comb4vec.Mag()/GeV;
        fillOffline &= !(vmag < 0.5);
      }
    }
    else  m_speaker->msg(MSG::INFO) << "No offline matches. fillOffline = " << fillOffline << endmsg;
    //else  break;    

    if(cutflow_check[2] && fillOffline) {
      cutflow_check[2] = 0;
      hmitr = m_histos.find("cutflow"); // is efficient
      if(hmitr != m_histos.end())  hmitr->second->Fill( 5.0 );
    }
    if(isTightPair && cutflow_check_tight[2] && fillOffline) {
      cutflow_check_tight[2] = 0;
      hmitr = m_histos.find("cutflow_tight"); // is efficient
      if(hmitr != m_histos.end())  hmitr->second->Fill( 5.0 );
    }

    ntracks = 0;
    vector<TIDA::Vertex*>::const_iterator it    = m_vertices.begin();
    vector<TIDA::Vertex*>::const_iterator itEnd = m_vertices.end();
    for(; it != itEnd; ++it)  ntracks += (*it)->Ntracks();

    // this is the dilepton invariant mass
    invmass = probe->mass()/GeV;

    pTt  = fabs(probe->pT());
    z0t  = probe->z0();
    etat = probe->eta();
    phit = fixPhi(probe->phi());
    /// use the tag variables for impact parameter, since the probe is extrapolated
    d0t = tag->a0();
    a0t = tag->a0() + sin(phit)*m_xBeamReference - cos(phit)*m_yBeamReference; 

    //q = pTt>0? 1. : -1.;

    float tnpDeltaR = deltaR(etat, phit, tag->eta(), tag->phi());

    /////////////////////////////////////
    /////////////////////////////////////
    //    if(tnpDeltaR < 0.1) continue;
    /////////////////////////////////////
    /////////////////////////////////////

    float dPhiTP = deltaPhi(tag->phi(), probe->phi()) / M_PI;

    hmitr = m_histos.find("tagAndProbe_invmass");
    if(hmitr != m_histos.end())  hmitr->second->Fill(invmass);

    if(m_offlineTags->at(i)) {
      hmitr = m_histos.find("tagMatch_invmass");
      if(hmitr != m_histos.end())  hmitr->second->Fill(invmass);
    }
      
    const int vpartSize = 6;
    float vpart[vpartSize] = { fabs(offpT)/GeV, offeta, offphi0, offz0, d0t, offd0 };
    for(int it = 0; it<vpartSize; it++) { 
      if(TH1F* hptr = find(varName[it]))  hptr->Fill(vpart[it]); 
    }  

    int isEfficient = 0, isEfficient_nocut = 0;
    float tinvmass = 0, ttMass = 0.;
    TIDA::Track *matched = 0, *matched_nocut = 0;
    double matched_eta = 0., matched_phi = 0.;
    //       matched_eta_nocut = 0., matched_phi_nocut = 0.;
    //int matched_roi = -1;

    float tempdmass = 999999.;
    float fillmass = 0.;

    float minDR = 0., minDR_nocut = 0., etaID = 0.,
          phiID = 0., ptID = 0., maxPt = 0.;
    m_speaker->msg(MSG::DEBUG) << "testtracks size: " << testtracks.size()
                               << " roi size: " << m_trackRois->size() << endmsg;
    for(unsigned int trigind = 0; trigind < testtracks.size(); trigind++) {
      // For electrons, require EQUAL RoIs for ID track and probe cluster
      // For muons, require OPPOSITE RoIs for ID track and probe cluster
      m_speaker->msg(MSG::DEBUG) << "testtrack [ pt = " << testtracks[trigind]->pT()
                                 << " eta = " << testtracks[trigind]->eta()
                                 << " phi = " << testtracks[trigind]->phi()
                                 << " roi = " << m_trackRois->operator[](trigind) << " ]" << endmsg;
      m_speaker->msg(MSG::DEBUG) << "track ROI: " << m_trackRois->operator[](trigind)
                                 << " probeRoiId: " << probeRoiId << endmsg;
      cout << "testtrack [ pt = " << testtracks[trigind]->pT()
           << " eta = " << testtracks[trigind]->eta()
           << " phi = " << testtracks[trigind]->phi()
           << " roi = " << m_trackRois->operator[](trigind) << " ]" << endl;
      m_speaker->msg(MSG::DEBUG) << "track ROI: " << m_trackRois->operator[](trigind)
                                 << " probeRoiId: " << probeRoiId << endmsg;
      cout << "track ROI: " << m_trackRois->operator[](trigind) << " probeRoiId: " << probeRoiId << endl;
      /*if(      m_isMuon && m_trackRois->operator[](trigind) == tagRoiId)    continue;
      else if(!m_isMuon && m_trackRois->operator[](trigind) != probeRoiId)  continue;*/
      if(m_trackRois->operator[](trigind) != probeRoiId)  continue;
      m_speaker->msg(MSG::INFO) << "test track passed RoI check!" << endmsg;
      cout << "test track passed RoI check!" << endl;

      TLorentzVector tagVec, trackVec, tagAndTrackPair;
      float Mass = m_isMuon? 105.66 : 0.511;
	
      float idEta   = testtracks[trigind]->eta();
      float idPhi   = fixPhi(testtracks[trigind]->phi());
      //float thetaId = 2. * atan(exp(-testtracks[trigind]->eta()));
      float idTheta = 2. * atan(exp(-idEta));
      float idPt    = testtracks[trigind]->pT();
      float idPx    = idPt * cos(idPhi);
      float idPy    = idPt * sin(idPhi);
      float idPz    = idPt * cos(idTheta) / sin(idTheta);
      float idEn    = sqrt(Mass*Mass + idPx*idPx + idPy*idPy + idPz*idPz);

      // TEMP !!!
      //
      //if( fabs(idPt) < 2000. )  continue;
      //
      // TEMP !!!

      float tagEn = sqrt(Mass*Mass + tag->px()*tag->px() + tag->py()*tag->py() + tag->pz()*tag->pz());

      tagVec.SetPtEtaPhiE(tag->pt(), tag->eta(), fixPhi(tag->phi()), tagEn);
      trackVec.SetPtEtaPhiE(idPt, idEta, idPhi, idEn);
      tagAndTrackPair = tagVec + trackVec;
      tinvmass = tagAndTrackPair.Mag();
      cout << "inv mass: " << (tinvmass / GeV)  << endl;

      /// select the most J/Psi-like pair
      if(fabs(tinvmass-mass) < tempdmass) { 
        tempdmass = fabs(tinvmass - mass);
        fillmass = tinvmass;
      }

      if(m_isMuon) {
        // (mass-m_minusMass) &&  (mass+m_plusMass)
        // this mass window matching is less than ideal
        // need to come up with something cleverer!!
        if(fabs(testtracks[trigind]->pT()) > maxPt) {
        //float mdR = deltaR(probe->eta(), probe->phi(), testtracks[trigind]->eta(), testtracks[trigind]->phi());
        //if(!minDR || mdR < minDR) {
        //
        /*float mdR = deltaR(probe->eta(), probe->phi(), testtracks[trigind]->eta(), testtracks[trigind]->phi());
        m_speaker->msg(MSG::DEBUG) << "mdR(probe, track): " << mdR << " cut: 0.4" << endmsg;
        if(!minDR_nocut || (minDR_nocut && mdR < minDR_nocut))  minDR_nocut = mdR;
        if(mdR < 0.1) {
          if(!isEfficient || (isEfficient && mdR < minDR)) {*/
          //isEfficient = 1;
          ttMass = tinvmass;
          matched = testtracks[trigind];
          etaID = testtracks[trigind]->eta();
          phiID = fixPhi(testtracks[trigind]->phi());
          matched_eta = testtracks[trigind]->eta();
          matched_phi = fixPhi(testtracks[trigind]->phi());
          ptID  = fabs(testtracks[trigind]->pT())/GeV;
          maxPt = fabs(testtracks[trigind]->pT());
          //minDR = mdR;
        }
      }
      else if(fillOffline) {
        float mdR = deltaR(offeta, offphi, testtracks[trigind]->eta(), testtracks[trigind]->phi());
        // as long as there is any track in the RoI it is efficient
        // use closest track as the 'matched track'
        if(!isEfficient_nocut || (isEfficient_nocut && mdR < minDR_nocut)) {
          matched_nocut = testtracks[trigind];
          //matched_phi_nocut = fixPhi(m_phiAtCalo->operator[](trigind));
          //matched_eta_nocut = m_etaAtCalo->operator[](trigind);
          minDR_nocut = mdR;
          isEfficient_nocut = 1; 
        }
        m_speaker->msg(MSG::DEBUG) << "mdR(off, track): " << mdR << endmsg;
        if(mdR < 0.03) {
          if(!isEfficient || (isEfficient && mdR < minDR)) {
            //etaID = m_etaAtCalo->operator[](trigind);
            //phiID = fixPhi(m_phiAtCalo->operator[](trigind));
            etaID = testtracks[trigind]->eta();
            phiID = fixPhi(testtracks[trigind]->phi());
            ptID  = fabs(testtracks[trigind]->pT())/GeV;
            matched = testtracks[trigind];
            matched_eta = testtracks[trigind]->eta();
            matched_phi = fixPhi(testtracks[trigind]->phi());
            //matched_phi = fixPhi(m_phiAtCalo->operator[](trigind));
            //matched_eta = m_etaAtCalo->operator[](trigind);
            //matched_roi = m_trackRois->operator[](trigind);
            minDR = mdR;
        	  isEfficient = 1; 
          }
        }
      } // else if (fillOffline)
    } // loop over all tracks

    if(m_isMuon) {
      double lowerBound = m_isJpsi?  1000. :  50000.;
      double upperBound = m_isJpsi? 20000  : 400000.;
      m_speaker->msg(MSG::DEBUG) << "mass(ID track, tag): " << ttMass << endmsg;
      m_speaker->msg(MSG::DEBUG) << "massLimitLow:  " << lowerBound << endmsg;
      m_speaker->msg(MSG::DEBUG) << "massLimitHigh: " << upperBound << endmsg;
      cout << "mass(ID track, tag): " << ttMass << endl;
      cout << "massLimitLow:  " << lowerBound << endl;
      cout << "massLimitHigh: " << upperBound << endl;
      hmitr = m_histos.find("invmass_tt");
      if(hmitr != m_histos.end())  hmitr->second->Fill( ttMass/GeV );
      if(!isEfficient && ttMass > lowerBound && ttMass < upperBound)  isEfficient = 1;
      else {
        matched = 0;
        etaID = phiID = ptID = 0.;
      }
    }
    
    if(isEfficient) {
      cout << "\033[92;1mEvent #: " << eventNumber << ", run #: "
           << runNumber << " --> is efficient! :-)\033[m" << endl;
      cout << "SKIM EFFICIENT -- event #: " << eventNumber
           << " run #: " << runNumber << endl;
      hmitr = m_histos.find("pT_track");
      if(hmitr != m_histos.end())  hmitr->second->Fill( ptID );
      hmitr = m_histos.find("eta_track");
      if(hmitr != m_histos.end())  hmitr->second->Fill( etaID );
      hmitr = m_histos.find("phi_track");
      if(hmitr != m_histos.end())  hmitr->second->Fill( phiID );
      hmitr = m_histos.find("dphi_tp");
      if(hmitr != m_histos.end())  hmitr->second->Fill( dPhiTP );

      if(fabs(dPhiTP) > 0.75) {
        hmitr = m_histos.find("pT_track_b2b");
        if(hmitr != m_histos.end())  hmitr->second->Fill( ptID );
        hmitr = m_histos.find("eta_track_b2b");
        if(hmitr != m_histos.end())  hmitr->second->Fill( etaID );
        hmitr = m_histos.find("phi_track_b2b");
        if(hmitr != m_histos.end())  hmitr->second->Fill( phiID );
      }
    }
    else {
      cout << "\033[91;1mEvent #: " << eventNumber << ", run #: "
           << runNumber << " --> is not efficient. :-(\033[m" << endl;
      cout << "SKIM INEFFICIENT -- event #: " << eventNumber
           << " run #: " << runNumber << endl;
    }

    if((probe->pT()/GeV) > 60. && !isEfficient && fillOffline) {
      cout << "\033[92;1mHIGH PT INEFFICIENCY: INVESTIGATE!!!\033[m" << endl;
    }

    if(minDR_nocut) {
      hmitr = m_histos.find("minDr_res");
      if(hmitr != m_histos.end())  hmitr->second->Fill( minDR_nocut );
    }

    if(cutflow_check[3] && isEfficient) {
      cutflow_check[3] = 0;
      hmitr = m_histos.find("cutflow"); // is efficient
      if(hmitr != m_histos.end())  hmitr->second->Fill( 6.0 );
    }
    else if(pTt > 30000.) { //DEBUG high pT inefficiency
      if(isTightPair && cutflow_check_tight[3] && isEfficient) {
        cutflow_check_tight[3] = 0;
        hmitr = m_histos.find("cutflow_tight"); // is efficient
        if(hmitr != m_histos.end())  hmitr->second->Fill( 6.0 );
      }

      if(fillOffline) {
        hmitr2d = m_histos2d.find("pt_vs_offpt");
        if(hmitr2d != m_histos2d.end())  hmitr2d->second->Fill(offpT/GeV, pTt/GeV, 1.0); // for most J/Psi-like, use 'fillmass'
      }

      hmitr = m_histos.find("tinvmass_miss");
      if(hmitr!=m_histos.end())  hmitr->second->Fill(fillmass/GeV);

    }
    /////////////DEBUG high pT inefficiency

    ///if you don't want to make any matching cut...
    //      isEfficient = 1;
    m_pT_eff->Fill(pTt/GeV,      isEfficient, 1.);
    m_eta_eff->Fill(etat,        isEfficient, 1.);
    m_vert_eff->Fill(nvert,      isEfficient, 1.);
    m_z0_eff->Fill(z0t,          isEfficient, 1.);
    m_d0_eff->Fill(a0t,          isEfficient, 1.);
    m_phi_eff->Fill(phit,        isEfficient, 1.);
    m_track_eff->Fill(ntracks,   isEfficient, 1.);
    m_invmass_eff->Fill(invmass, isEfficient, 1.);


    hmitr2d = m_histos2d.find("invmass_vs_tinvmass");
    if(hmitr2d != m_histos2d.end())  hmitr2d->second->Fill(fillmass/GeV, invmass, 1.0); // for most J/Psi-like, use 'fillmass'
    hmitr2d = m_histos2d.find("invmass_vs_deltar");
    if(hmitr2d != m_histos2d.end())  hmitr2d->second->Fill(tnpDeltaR,    invmass, 1.0); // for most J/Psi-like, use 'fillmass'
    hmitr2d = m_histos2d.find("pt_vs_deltar");
    if(hmitr2d != m_histos2d.end())  hmitr2d->second->Fill(tnpDeltaR,    pTt/GeV, 1.0); // for most J/Psi-like, use 'fillmass'

    //////FOR ONLINE-OFFLINE COMPARISONS ONLY
    //    offpT = pTt;
    //    offeta = etat;
    //    offz0 = z0t;
    //    offphi0 = phit;
      
    ///temp
    if(fabs(offd0) > 1.0) {
      m_pT_eff_d0cut->Fill(offpT/GeV,    isEfficient, 1.);
      m_eta_eff_d0cut->Fill(offeta,      isEfficient, 1.);
      m_vert_eff_d0cut->Fill(nvert,      isEfficient, 1.);
      m_z0_eff_d0cut->Fill(offz0,        isEfficient, 1.);
      m_d0_eff_d0cut->Fill(offd0,        isEfficient, 1.);
      m_phi_eff_d0cut->Fill(offphi0,     isEfficient, 1.);
      m_track_eff_d0cut->Fill(ntracks,   isEfficient, 1.);
      m_invmass_eff_d0cut->Fill(invmass, isEfficient, 1.);
    }

    static const int num = 11;
    static string effName[num] = { "pT", "eta", "phi", "d0", "absd0", "z0", "absz0",
                                   "invmass", "track", "vert", "mu" };
    float effVar[num] = { fabs(pTt)/GeV, etat, phit, offd0, fabs(offd0), offz0,
                          fabs(offz0), invmass, float(ntracks), float(nvert), mu_inTime };

    if(isEfficient) {
      for (int it = 0; it<num; it++) { 
        if(TH1F* hptr = find(effName[it]+"num"))  hptr->Fill(effVar[it]); 
      }  
      for(int it = 0; it<num; it++) {
	      if(TH1F* hptr = find(effName[it]+"den"))  hptr->Fill(effVar[it]); 
      }  
      if(TH1F* hptr = find("deltarnum"))  hptr->Fill(tnpDeltaR);
      if(TH1F* hptr = find("deltarden"))  hptr->Fill(tnpDeltaR);
      if(TH1F* hptr = find("ETnum"))  hptr->Fill( matched_brem );
      if(TH1F* hptr = find("ETden"))  hptr->Fill( matched_brem );
      if(fabs(offd0) < 0.5) {
        if(TH1F* hptr = find("pTnum_d0cut"))  hptr->Fill(pTt/GeV);
	      if(TH1F* hptr = find("pTden_d0cut"))  hptr->Fill(pTt/GeV);
      }
      if(fabs(pTt) < 20000.) {
        if(TH1F* hptr = find("deltarnum_lowpt"))  hptr->Fill(tnpDeltaR);
        if(TH1F* hptr = find("deltarden_lowpt"))  hptr->Fill(tnpDeltaR);
      }
      if(matched_brem > 0.9) {
        if( TH1F* hptr = find("pTEtnum") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("pTEtden") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("etaEtnum") ) hptr->Fill( etat );
        if( TH1F* hptr = find("etaEtden") ) hptr->Fill( etat );
      }
      else if(matched_brem < 0.5) {
        if( TH1F* hptr = find("pTbremnum") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("pTbremden") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("etabremnum") ) hptr->Fill( etat );
        if( TH1F* hptr = find("etabremden") ) hptr->Fill( etat );
      }
      if(fabs(dPhiTP) > 0.5) {
        if( TH1F* hptr = find("pTb2bloosenum") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("pTb2blooseden") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("etab2bloosenum") ) hptr->Fill( etat );
        if( TH1F* hptr = find("etab2blooseden") ) hptr->Fill( etat );
        if( TH1F* hptr = find("mub2bloosenum") ) hptr->Fill( mu_inTime );
        if( TH1F* hptr = find("mub2blooseden") ) hptr->Fill( mu_inTime );
        if(fabs(dPhiTP) > 0.75) {
          if( TH1F* hptr = find("pTb2bnum") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("pTb2bden") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("etab2bnum") ) hptr->Fill( etat );
          if( TH1F* hptr = find("etab2bden") ) hptr->Fill( etat );
          if( TH1F* hptr = find("mub2bnum") ) hptr->Fill( mu_inTime );
          if( TH1F* hptr = find("mub2bden") ) hptr->Fill( mu_inTime );
        }
      }
    }
    else {
      hmitr = m_histos.find("tagAndProbe_invmass_wo_match");
      if(hmitr!=m_histos.end())  hmitr->second->Fill(invmass);
      ///temp, pT of missed offline track
      hmitr = m_histos.find("pTmiss");
      if(hmitr!=m_histos.end())  hmitr->second->Fill(fabs(offpT/GeV));
      
      for ( int it=0 ; it<num; it++ ) { 
        if(TH1F* hptr = find(effName[it]+"den"))  hptr->Fill(effVar[it]); 
      }  
      if(TH1F* hptr = find("deltarden"))  hptr->Fill(tnpDeltaR);
      if(TH1F* hptr = find("ETden"))  hptr->Fill( matched_brem );
      if(fabs(offd0) < 0.5) { 
        if(TH1F* hptr = find("pTden_d0cut"))  hptr->Fill(pTt/GeV);
      }
      if(fabs(pTt) < 20000.)  {
        if(TH1F* hptr = find("deltarden_lowpt"))  hptr->Fill(tnpDeltaR);
      }
      if(matched_brem > 0.9) {
        if( TH1F* hptr = find("pTEtden") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("etaEtden") ) hptr->Fill( etat );
      }
      else if(matched_brem < 0.5) {
        if( TH1F* hptr = find("pTbremden") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("etabremden") ) hptr->Fill( etat );
      }
      if(fabs(dPhiTP) > 0.5) {
        if( TH1F* hptr = find("pTb2blooseden") )  hptr->Fill( fabs(pTt)/GeV );
        if( TH1F* hptr = find("etab2blooseden") ) hptr->Fill( etat );
        if( TH1F* hptr = find("mub2blooseden") ) hptr->Fill( mu_inTime );
        if(fabs(dPhiTP) > 0.75) {
          if( TH1F* hptr = find("pTb2bden") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("etab2bden") ) hptr->Fill( etat );
          if( TH1F* hptr = find("mub2bden") ) hptr->Fill( mu_inTime );
        }
      }
    }

    if(fillOffline) {
      hmitr = m_histos.find("pT_offtrack");
      if(hmitr != m_histos.end())  hmitr->second->Fill( fabs(offpT)/GeV );
      hmitr = m_histos.find("eta_offtrack");
      if(hmitr != m_histos.end())  hmitr->second->Fill( offeta );
      hmitr = m_histos.find("phi_offtrack");
      if(hmitr != m_histos.end())  hmitr->second->Fill( offphi );
      if(matched_brem > 0.9) {
        hmitr = m_histos.find("pT_offtrackEt");
        if(hmitr != m_histos.end())  hmitr->second->Fill( fabs(offpT)/GeV );
        hmitr = m_histos.find("eta_offtrackEt");
        if(hmitr != m_histos.end())  hmitr->second->Fill( offeta );
        hmitr = m_histos.find("phi_offtrackEt");
        if(hmitr != m_histos.end())  hmitr->second->Fill( offphi );
      }
      else if(matched_brem < 0.5) {
        hmitr = m_histos.find("pT_offtrackbrem");
        if(hmitr != m_histos.end())  hmitr->second->Fill( fabs(offpT)/GeV );
        hmitr = m_histos.find("eta_offtrackbrem");
        if(hmitr != m_histos.end())  hmitr->second->Fill( offeta );
        hmitr = m_histos.find("phi_offtrackbrem");
        if(hmitr != m_histos.end())  hmitr->second->Fill( offphi );
      }

      m_pT_eff_offline->Fill(offpT/GeV,    isEfficient, 1.);
      m_eta_eff_offline->Fill(offeta,      isEfficient, 1.);
      m_vert_eff_offline->Fill(nvert,      isEfficient, 1.);
      m_z0_eff_offline->Fill(offz0,        isEfficient, 1.);
      m_d0_eff_offline->Fill(offd0,        isEfficient, 1.);
      m_phi_eff_offline->Fill(offphi0,     isEfficient, 1.);
      m_track_eff_offline->Fill(ntracks,   isEfficient, 1.);
      m_invmass_eff_offline->Fill(invmass, isEfficient, 1.);

      ///temp
      if(fabs(offd0) > 1.0){
        m_pT_eff_d0cut_offline->Fill(offpT/GeV,    isEfficient, 1.);
        m_eta_eff_d0cut_offline->Fill(offeta,      isEfficient, 1.);
        m_vert_eff_d0cut_offline->Fill(nvert,      isEfficient, 1.);
        m_z0_eff_d0cut_offline->Fill(offz0,        isEfficient, 1.);
        m_d0_eff_d0cut_offline->Fill(offd0,        isEfficient, 1.);
        m_phi_eff_d0cut_offline->Fill(offphi0,     isEfficient, 1.);
        m_track_eff_d0cut_offline->Fill(ntracks,   isEfficient, 1.);
        m_invmass_eff_d0cut_offline->Fill(invmass, isEfficient, 1.);
      }

      if(isEfficient) {
        for(int it = 0; it<num; it++) { 
          if(TH1F* hptr = find(effName[it]+"num_off"))  hptr->Fill(effVar[it]); 
        }  
        for(int it = 0; it<num; it++) { 
          if(TH1F* hptr = find(effName[it]+"den_off"))  hptr->Fill(effVar[it]); 
        }  
        if(TH1F* hptr = find("pToffnum_off"))  hptr->Fill(fabs(offpT/GeV));
        if(TH1F* hptr = find("pToffden_off"))  hptr->Fill(fabs(offpT/GeV));
        if(fabs(offd0) < 0.5) {
          if( TH1F* hptr = find("pToffnum_d0cut") ) hptr->Fill( offpT/GeV );
          if( TH1F* hptr = find("pToffden_d0cut") ) hptr->Fill( offpT/GeV );
        }
        if( TH1F* hptr = find("pTofftracknum_off") )  hptr->Fill( fabs(offpT)/GeV );
        if( TH1F* hptr = find("pTofftrackden_off") )  hptr->Fill( fabs(offpT)/GeV );
        if( TH1F* hptr = find("etaofftracknum_off") )  hptr->Fill( offeta );
        if( TH1F* hptr = find("etaofftrackden_off") )  hptr->Fill( offeta );
        if( TH1F* hptr = find("ETnum_off") )  hptr->Fill( matched_brem );
        if( TH1F* hptr = find("ETden_off") )  hptr->Fill( matched_brem );
        if(matched_brem > 0.9) {
          if( TH1F* hptr = find("pTEtnum_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("pTEtden_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("etaEtnum_off") ) hptr->Fill( etat );
          if( TH1F* hptr = find("etaEtden_off") ) hptr->Fill( etat );
          if( TH1F* hptr = find("pTofftrackEtnum_off") )  hptr->Fill( fabs(offpT)/GeV );
          if( TH1F* hptr = find("pTofftrackEtden_off") )  hptr->Fill( fabs(offpT)/GeV );
          if( TH1F* hptr = find("etaofftrackEtnum_off") ) hptr->Fill( offeta );
          if( TH1F* hptr = find("etaofftrackEtden_off") ) hptr->Fill( offeta );
        }
        else if(matched_brem < 0.5) {
          if( TH1F* hptr = find("pTbremnum_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("pTbremden_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("etabremnum_off") )  hptr->Fill( etat );
          if( TH1F* hptr = find("etabremden_off") )  hptr->Fill( etat );
          if( TH1F* hptr = find("pTofftrackbremnum_off") )  hptr->Fill( fabs(offpT)/GeV );
          if( TH1F* hptr = find("pTofftrackbremden_off") )  hptr->Fill( fabs(offpT)/GeV );
          if( TH1F* hptr = find("etaofftrackbremnum_off") )  hptr->Fill( offeta );
          if( TH1F* hptr = find("etaofftrackbremden_off") )  hptr->Fill( offeta );
        }
        if(fabs(dPhiTP) > 0.5) {
          if( TH1F* hptr = find("pTb2bloosenum_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("pTb2blooseden_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("etab2bloosenum_off") ) hptr->Fill( etat );
          if( TH1F* hptr = find("etab2blooseden_off") ) hptr->Fill( etat );
          if( TH1F* hptr = find("mub2bloosenum_off") ) hptr->Fill( mu_inTime );
          if( TH1F* hptr = find("mub2blooseden_off") ) hptr->Fill( mu_inTime );
          if(fabs(dPhiTP) > 0.75) {
            if( TH1F* hptr = find("pTb2bnum_off") )  hptr->Fill( fabs(pTt)/GeV );
            if( TH1F* hptr = find("pTb2bden_off") )  hptr->Fill( fabs(pTt)/GeV );
            if( TH1F* hptr = find("etab2bnum_off") ) hptr->Fill( etat );
            if( TH1F* hptr = find("etab2bden_off") ) hptr->Fill( etat );
            if( TH1F* hptr = find("mub2bnum_off") ) hptr->Fill( mu_inTime );
            if( TH1F* hptr = find("mub2bden_off") ) hptr->Fill( mu_inTime );
          }
        }
      }    
      else {
        for(int it = 0; it<num; it++) { 
          if(TH1F* hptr = find(effName[it]+"den_off"))  hptr->Fill(effVar[it]); 
        }  
  	    if(TH1F* hptr = find("pToffden_off"))  hptr->Fill(fabs(offpT/GeV));
        if(TH1F* hptr = find("ETden_off"))     hptr->Fill( matched_brem );
        if(fabs(offd0) < 0.5) {
          if(TH1F* hptr = find("pToffden_d0cut"))  hptr->Fill(offpT/GeV);
        }
        if( TH1F* hptr = find("pTofftrackden_off") )  hptr->Fill( fabs(offpT)/GeV );
        if( TH1F* hptr = find("etaofftrackden_off") ) hptr->Fill( offeta );
        if(matched_brem > 0.9) {
          if( TH1F* hptr = find("pTEtden_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("etaEtden_off") ) hptr->Fill( etat );
          if( TH1F* hptr = find("pTofftrackEtden_off") )  hptr->Fill( fabs(offpT)/GeV );
          if( TH1F* hptr = find("etaofftrackEtden_off") ) hptr->Fill( offeta );
        }
        else if(matched_brem < 0.5) {
          if( TH1F* hptr = find("pTbremden_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("etabremden_off") ) hptr->Fill( etat );
          if( TH1F* hptr = find("pTofftrackbremden_off") )  hptr->Fill( fabs(offpT)/GeV );
          if( TH1F* hptr = find("etaofftrackbremden_off") ) hptr->Fill( offeta );
        }
        if(fabs(dPhiTP) > 0.5) {
          if( TH1F* hptr = find("pTb2blooseden_off") )  hptr->Fill( fabs(pTt)/GeV );
          if( TH1F* hptr = find("etab2blooseden_off") ) hptr->Fill( etat );
          if( TH1F* hptr = find("mub2blooseden_off") ) hptr->Fill( mu_inTime );
          if(fabs(dPhiTP) > 0.75) {
            if( TH1F* hptr = find("pTb2bden_off") )  hptr->Fill( fabs(pTt)/GeV );
            if( TH1F* hptr = find("etab2bden_off") ) hptr->Fill( etat );
            if( TH1F* hptr = find("mub2bden_off") ) hptr->Fill( mu_inTime );
          }
        }
      }
    }
      
    // residual histos, between ID track and probe
    if(isEfficient && matched) {

      float pTr  = fabs(matched->pT());  
      float phir = fixPhi(matched_phi);
      float etar = matched_eta;
      float z0r  = matched->z0(); 
      float d0r  = matched->a0(); 
      float a0r  = matched->a0() + sin(phir)*m_xBeamTest - cos(phir)*m_yBeamTest; // this will be changed when we know the beam spot position

      string vtname[6] = { "pT_rec", "eta_rec", "phi_rec", "z0_rec", "d0_rec", "a0_rec"};
      float vtpart[6] = { fabs(pTr)/GeV, etar, phir, z0r, d0r, a0r };

      for(int it=0; it<6; it++) { 
	      if(TH1F* hptr = find(vtname[it]))  hptr->Fill(vtpart[it]); 
      }  
      
      float dphi = deltaPhi(phit, phir);

      float vres[6] = { 1/pTt-1/pTr, etat-etar, dphi, z0t-z0r, d0t-d0r, a0t-a0r };
       
      for(int it=0; it<6; it++) { 
        if(TH1F* hptr = find(varName[it]+"_res"))  hptr->Fill(vres[it]); 
      }  
      float dr = deltaR(etat, phit, etar, phir);
      hmitr = m_histos.find("deltar");
      if(hmitr!=m_histos.end())  hmitr->second->Fill(dr);
      if(fillOffline) {
        hmitr2d = m_histos2d.find("asymEta");
        if(hmitr2d != m_histos2d.end())  hmitr2d->second->Fill(offeta, matched->eta());
        hmitr2d = m_histos2d.find("asymPhi");
        if(hmitr2d != m_histos2d.end())  hmitr2d->second->Fill(offphi, fixPhi(matched->phi()));
      }
    }
    if(fillOffline && isEfficient_nocut && matched_nocut) {
      float dEtaProbeOff = offeta  - matched_nocut->eta();
      float dPhiProbeOff = deltaPhi(offphi, matched_nocut->phi());
      /*if(fabs(dEtaProbeOff) > 0.4 || fabs(dPhiProbeOff) > 0.4) {
        cout << "OUTLIER INFO: tag -- pt = " << tag->pt()
             << " eta = " << tag->eta()
             << " phi = " << tag->phi()
             << " roi = " << tag->roiNum() << endl;
        cout << "OUTLIER INFO: probe -- pt = " << probe->pT()
             << " eta = " << probe->eta()
             << " phi = " << probe->phi()
             << " roi = " << probe->getRoiNum() << endl;
        cout << "OUTLIER INFO: track-- pt = " << matched_nocut->pT()
             << " eta = " << matched_nocut->eta() << " (" << matched_eta_nocut << ")"
             << " phi = " << matched_nocut->phi() << " (" << matched_phi_nocut << ")" << endl;
        cout << "OUTLIER INFO: offline probe track-- pt = " << offpT
             << " eta = " << offeta
             << " phi = " << offphi << endl;
      }*/
      hmitr = m_histos.find("etaOff_res");
      if(hmitr != m_histos.end())  hmitr->second->Fill(dEtaProbeOff);
      hmitr = m_histos.find("phiOff_res");
      if(hmitr != m_histos.end())  hmitr->second->Fill(dPhiProbeOff);
      hmitr = m_histos.find("rOff_res");
      if(hmitr != m_histos.end())  hmitr->second->Fill(minDR_nocut);
    }
  } 
  m_speaker->msg(MSG::DEBUG) << "... end of JpsiAnalysis execute." << endmsg;
}

// ###########################################################################
 
float JpsiAnalysis::deltaR(float eta1, float phi1, float eta2, float phi2) {
  float deta = eta1-eta2;
  float dphi = deltaPhi(phi1,phi2);
  return TMath::Sqrt(deta*deta+dphi*dphi);
}
 
// ###########################################################################

float JpsiAnalysis::deltaPhi(float phi1, float phi2) {
  if(TMath::IsNaN(phi1)||TMath::IsNaN(phi2)) {
    cout << "> > > > > WTF!? deltaPhi(phi1,phi2) called with NaN" << endl;
  }
  return fixPhi(phi1-phi2);
}

// ###########################################################################
 
float JpsiAnalysis::fixPhi(float phi) {
  while(phi >=  M_PI) phi -= 2.0*M_PI;
  while(phi <  -M_PI) phi += 2.0*M_PI;
  return phi;
}
 
// ###########################################################################

