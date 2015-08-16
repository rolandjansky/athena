/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoMuonPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"
typedef ElementLink< xAOD::TrackParticleContainer > TrackLink;
typedef ElementLink< xAOD::MuonContainer > MuonLink;

RecoMuonPlots::RecoMuonPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir)

, m_oAllPlots(this, "/", "Reco Muon")
, m_oMuRecoInfoPlots(this, "/")
, m_oImpactPlots(this, "/")
, m_oMomentumPullPlots(this, "/")
, m_oMSHitPlots(this,"/")
, m_oMuonHitSummaryPlots(this,"/")
, m_oMuonIsolationPlots(this,"/")
, m_oMuonParamPlots(this,"/")
, m_oAllPlots_tight(this, "/Tight/", "Tight")
, m_oAllPlots_medium(this, "/Medium/", "Medium")
, m_oAllPlots_loose(this, "/Loose/", "Loose")
, m_oAllPlots_veryloose(this, "/Veryloose/", "Veryloose")
, m_eff_tight(NULL)
, m_eff_medium(NULL)
, m_eff_loose(NULL)
, m_eff_veryloose(NULL)
, m_pt_broad(NULL)
{}

void RecoMuonPlots::initializePlots(){
  //be very careful here, bin size is the same as the defult value
  //std::vector<HistData> hists = m_oAllPlots.retrieveBookedHistograms(); // HistData -> std::pair<TH1*, std::string>
  int xbins = 64;
  int ybins = 128;
  float xmin = -3.2;
  float xmax = 3.2;
  float ymin = -3.2;
  float ymax = 3.2;
  // for (auto hist: hists) {
  //   TString sHistName = hist.first->GetName();
  //   TString sHistTitle = hist.first->GetTitle();
  //   //change the axis get label
  //   if (sHistName.Contains("_eta_phi")){
  //     xmin = hist.first->GetXaxis()->GetXmin();
  //     xmax = hist.first->GetXaxis()->GetXmax();
  //     ymin = hist.first->GetYaxis()->GetXmin();
  //     ymax = hist.first->GetYaxis()->GetXmax();
  //     xbins = hist.first->GetXaxis()->GetNbins();
  //     ybins = hist.first->GetXaxis()->GetNbins();
  //   }
  // }
  //now register!
  m_eff_tight     = Book2D("_Tight_eff",          "Tight Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_eff_medium    = Book2D("_Medium_eff",        "Medium Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_eff_loose     = Book2D("_Loose_eff",          "Loose Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_eff_veryloose = Book2D("_Veryloose_eff", "Very Loose Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_pt_broad = Book1D("_pt_broad", "High p_{T} Distribution", 140, 100, 1500);
}

void RecoMuonPlots::fill(const xAOD::Muon& mu){
  //General Plots
  m_oAllPlots.fill(mu);
  m_oMuRecoInfoPlots.fill(mu);
  m_oMomentumPullPlots.fill(mu);
  m_oMuonHitSummaryPlots.fill(mu);
  m_oMuonIsolationPlots.fill(mu);
  m_oMuonParamPlots.fill(mu);
  m_pt_broad->Fill(mu.pt()/1000);

  // tracking related plots
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (!primaryTrk) return;
  m_oImpactPlots.fill(*primaryTrk);
  m_oMSHitPlots.fill(*primaryTrk);
}

void RecoMuonPlots::fill(const xAOD::Muon& mu, xAOD::Muon::Quality my_quality){
  //General Plots; inclusive
  if (my_quality <= xAOD::Muon::Tight) m_oAllPlots_tight.fill(mu);
  if (my_quality <= xAOD::Muon::Medium) m_oAllPlots_medium.fill(mu);
  if (my_quality <= xAOD::Muon::Loose) m_oAllPlots_loose.fill(mu);
  if (my_quality <= xAOD::Muon::VeryLoose) m_oAllPlots_veryloose.fill(mu);
}
