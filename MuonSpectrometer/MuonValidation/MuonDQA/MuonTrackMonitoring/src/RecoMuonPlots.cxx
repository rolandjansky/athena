/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoMuonPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"
typedef ElementLink< xAOD::TrackParticleContainer > TrackLink;
typedef ElementLink< xAOD::MuonContainer > MuonLink;

RecoMuonPlots::RecoMuonPlots(PlotBase* pParent, std::string sDir, bool detail):PlotBase(pParent, sDir)

, m_oAllPlots(this, "/", "Reco Muon")
, m_oMuRecoInfoPlots(this, "/")
, m_oImpactPlots(this, "/")
, m_oMomentumPullPlots(this, "/")
//, m_oMSHitPlots(this,"/")
//, m_oMuonHitSummaryPlots(this,"/")
//, m_oMuonIsolationPlots(this,"/")
//, m_oMuonParamPlots(this,"/")
//, m_oAllPlots_tight(this, "/Tight/", "Tight")
//, m_oAllPlots_medium(this, "/Medium/", "Medium")
//, m_oAllPlots_loose(this, "/Loose/", "Loose")
//, m_oAllPlots_veryloose(this, "/Veryloose/", "Veryloose")
, m_origin_eta_phi(NULL)
, m_eff_tight(NULL)
, m_eff_medium(NULL)
, m_tight_eta_phi(NULL)
, m_medium_eta_phi(NULL)
//, m_eff_loose(NULL)
//, m_eff_veryloose(NULL)
, m_pt_broad(NULL)
, m_eta_phi_broad(NULL)
, m_eff_nPrec(NULL)
, m_eff_nPhi(NULL)
, m_eff_nTrigEta(NULL)
, m_eff_ndof(NULL)
, m_eff_chi2(NULL)
, m_ID_eff_ndof(NULL)
, m_ID_eff_chi2(NULL)
, m_MS_eff_ndof(NULL)
, m_MS_eff_chi2(NULL)
{
  Detail = detail;
}

void RecoMuonPlots::initializePlots(){
  //be very careful here, bin size is the same as the defult value
  std::vector<HistData> hists = m_oAllPlots.retrieveBookedHistograms(); // HistData -> std::pair<TH1*, std::string>
  int xbins = 52;
  int ybins = 64;
  float xmin = -2.6;
  float xmax = 2.6;
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
  m_origin_eta_phi= Book2D("_Origin_eta_phi",        "Original Distribution;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_eff_tight     = Book2D("_Tight_eff",         "Tight Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_eff_medium    = Book2D("_Medium_eff",        "Medium Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_tight_eta_phi = Book2D("_Tight_eta_phi",     "Tight Muon #eta #phi Distribution;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_medium_eta_phi = Book2D("_Medium_eta_phi",    "Medium Muon #eta #phi Distribution;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  //m_eff_loose     = Book2D("_Loose_eff",          "Loose Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  //m_eff_veryloose = Book2D("_Veryloose_eff", "Very Loose Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  if (Detail){
    m_pt_broad      = Book1D("_pt_broad", "High p_{T} Distribution;p_{T} [GeV]", 70, 100, 1500);
    m_eta_phi_broad = Book2D("_eta_phi_broad", "High p_{T} Muon #eta #phi Distribution;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);

    m_eff_nPrec    = Book2D("_eff_nPrec",   "average number of Precision Layers;#eta;#phi",   xbins, xmin, xmax, ybins, ymin, ymax);
    m_eff_nPhi     = Book2D("_eff_nPhi",    "average number of Phi Layers;#eta;#phi",         xbins, xmin, xmax, ybins, ymin, ymax);
    m_eff_nTrigEta = Book2D("_eff_nTrigEta","average number of Trigger Eta Layers;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_eff_ndof     = Book2D("_eff_ndof",    "average number of hits;#eta;#phi",xbins, xmin, xmax, ybins, ymin, ymax);
    m_eff_chi2     = Book2D("_eff_chi2",    "average #chi^{2} per DOF;#eta;#phi",xbins, xmin, xmax, ybins, ymin, ymax);
    m_ID_eff_ndof  = Book2D("_ID_eff_ndof", "ID track average number of hits;#eta;#phi",xbins, xmin, xmax, ybins, ymin, ymax);
    m_ID_eff_chi2  = Book2D("_ID_eff_chi2", "ID track average #chi^{2} per DOF;#eta;#phi",xbins, xmin, xmax, ybins, ymin, ymax);
    m_MS_eff_ndof  = Book2D("_MS_eff_ndof", "MS track average number of hits;#eta;#phi",xbins, xmin, xmax, ybins, ymin, ymax);
    m_MS_eff_chi2  = Book2D("_MS_eff_chi2", "MS track average #chi^{2} per DOF;#eta;#phi",xbins, xmin, xmax, ybins, ymin, ymax);
  }
}

void RecoMuonPlots::fill(const xAOD::Muon& mu){
  //General Plots
  m_oAllPlots.fill(mu);
  m_oMuRecoInfoPlots.fill(mu);
  m_oMomentumPullPlots.fill(mu);
  //m_oMuonHitSummaryPlots.fill(mu);
  //m_oMuonIsolationPlots.fill(mu);
  //m_oMuonParamPlots.fill(mu);

  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  // tracking related plots
  if (!primaryTrk) return;
  m_oImpactPlots.fill(*primaryTrk);
  //m_oMSHitPlots.fill(*primaryTrk);

}

void RecoMuonPlots::fill(const xAOD::Muon& mu, xAOD::Muon::Quality my_quality){

  //always fill the origin plot
  m_origin_eta_phi->Fill(mu.eta(), mu.phi());

  //General Plots; inclusive
  if (my_quality <= xAOD::Muon::Tight) m_tight_eta_phi->Fill(mu.eta(), mu.phi());
  if (my_quality <= xAOD::Muon::Medium) m_medium_eta_phi->Fill(mu.eta(), mu.phi());
  //if (my_quality <= xAOD::Muon::Loose) m_oAllPlots_loose.fill(mu);
  //if (my_quality <= xAOD::Muon::VeryLoose) m_oAllPlots_veryloose.fill(mu);

  if(!Detail) return;
  if (mu.pt()/1000.0 > 100) {//ony for high pt muons
    m_pt_broad->Fill(mu.pt()/1000.0);
    m_eta_phi_broad->Fill(mu.eta(), mu.phi());
  }
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  const xAOD::TrackParticle* inDetTrk = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  int correctEnum = (int) xAOD::Muon::MuonSpectrometerTrackParticle;
  if (mu.isAvailable< ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink") && (mu.auxdata< ElementLink<xAOD::TrackParticleContainer> >("extrapolatedMuonSpectrometerTrackParticleLink")).isValid()) correctEnum+=2; //check correct numbering in Muon.h
  const xAOD::TrackParticle* msExtrapTrk = mu.trackParticle((xAOD::Muon::TrackParticleType) correctEnum);

  //fill the ndof and chi2 as eta phi map; to calculate efficiencies in post processing
  //if necessary, can use a quality cut here
  if (primaryTrk) {
    m_eff_ndof->Fill(primaryTrk->eta(), primaryTrk->phi(), primaryTrk->numberDoF());
    m_eff_chi2->Fill(primaryTrk->eta(), primaryTrk->phi(), primaryTrk->chiSquared()/(primaryTrk->numberDoF() * 1.0));
  }
  if (inDetTrk) {
    m_ID_eff_ndof->Fill(inDetTrk->eta(), inDetTrk->phi(), inDetTrk->numberDoF());
    m_ID_eff_chi2->Fill(inDetTrk->eta(), inDetTrk->phi(), inDetTrk->chiSquared()/(inDetTrk->numberDoF() * 1.0));
  }
  if (msExtrapTrk) {
    m_MS_eff_ndof->Fill(msExtrapTrk->eta(), msExtrapTrk->phi(), msExtrapTrk->numberDoF());
    m_MS_eff_chi2->Fill(msExtrapTrk->eta(), msExtrapTrk->phi(), msExtrapTrk->chiSquared()/(msExtrapTrk->numberDoF() * 1.0));
  }

  uint8_t hitval_numberOfPrecisionLayers = 0;
  if (!primaryTrk->summaryValue(hitval_numberOfPrecisionLayers, xAOD::numberOfPrecisionLayers)) return; 
  m_eff_nPrec->Fill(mu.eta(), mu.phi(), hitval_numberOfPrecisionLayers);
  uint8_t hitval_numberOfPhiLayers = 0;
  if (!primaryTrk->summaryValue(hitval_numberOfPhiLayers, xAOD::numberOfPhiLayers)) return; 
  m_eff_nPhi->Fill(mu.eta(), mu.phi(),  hitval_numberOfPhiLayers);
  uint8_t hitval_numberOfTriggerEtaLayers = 0;
  if (!primaryTrk->summaryValue(hitval_numberOfTriggerEtaLayers, xAOD::numberOfTriggerEtaLayers)) return; 
  m_eff_nTrigEta->Fill(mu.eta(), mu.phi(), hitval_numberOfTriggerEtaLayers);

}
