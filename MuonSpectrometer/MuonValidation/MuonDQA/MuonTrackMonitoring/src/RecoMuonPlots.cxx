/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
, m_origin_eta_phi(nullptr)
, m_eff_tight(nullptr)
, m_eff_medium(nullptr)
, m_tight_eta_phi(nullptr)
, m_medium_eta_phi(nullptr)
, m_pt_broad(nullptr)
, m_eta_phi_broad(nullptr)
, m_eff_nPrec(nullptr)
, m_eff_nPhi(nullptr)
, m_eff_nTrigEta(nullptr)
, m_eff_ndof(nullptr)
, m_eff_chi2(nullptr)
, m_ID_eff_ndof(nullptr)
, m_ID_eff_chi2(nullptr)
, m_MS_eff_ndof(nullptr)
, m_MS_eff_chi2(nullptr)
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

  //now register!
  m_origin_eta_phi= Book2D("_Origin_eta_phi",        "Original Distribution;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_eff_tight     = Book2D("_Tight_eff",         "Tight Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_eff_medium    = Book2D("_Medium_eff",        "Medium Quality Efficiency;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_tight_eta_phi = Book2D("_Tight_eta_phi",     "Tight Muon #eta #phi Distribution;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  m_medium_eta_phi = Book2D("_Medium_eta_phi",    "Medium Muon #eta #phi Distribution;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
  if (Detail){
    m_pt_broad      = Book1D("_pt_broad", "High p_{T} Distribution;p_{T} [GeV]", 70, 100, 1500);
    m_eta_phi_broad = Book2D("_eta_phi_broad", "High p_{T} Muon #eta #phi Distribution;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);

    m_eff_nPrec    = Book2D("_eff_nPrec",   "average number of Precision Layers;#eta;#phi",   xbins, xmin, xmax, ybins, ymin, ymax);
    m_eff_nPhi     = Book2D("_eff_nPhi",    "average number of Phi Layers;#eta;#phi",         xbins, xmin, xmax, ybins, ymin, ymax);
    m_eff_nTrigEta = Book2D("_eff_nTrigEta","average number of Trigger Eta Layers;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_eff_ndof     = Book2D("_eff_ndof",    "average number of hits;#eta;#phi",               xbins, xmin, xmax, ybins, ymin, ymax);
    m_eff_chi2     = Book2D("_eff_chi2",    "average #chi^{2} per DOF;#eta;#phi",             xbins, xmin, xmax, ybins, ymin, ymax);
    m_ID_eff_ndof  = Book2D("_ID_eff_ndof", "ID track average number of hits;#eta;#phi",      xbins, xmin, xmax, ybins, ymin, ymax);
    m_ID_eff_chi2  = Book2D("_ID_eff_chi2", "ID track average #chi^{2} per DOF;#eta;#phi",    xbins, xmin, xmax, ybins, ymin, ymax);
    m_MS_eff_ndof  = Book2D("_MS_eff_ndof", "MS track average number of hits;#eta;#phi",      xbins, xmin, xmax, ybins, ymin, ymax);
    m_MS_eff_chi2  = Book2D("_MS_eff_chi2", "MS track average #chi^{2} per DOF;#eta;#phi",    xbins, xmin, xmax, ybins, ymin, ymax);

    m_avg_hits_precision_inner    = Book2D("_avg_hits_precision_inner",    "avg. precision hits, inner;#eta;#phi",    xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_precision_middle   = Book2D("_avg_hits_precision_middle",   "avg. precision hits, middle;#eta;#phi",   xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_precision_outer    = Book2D("_avg_hits_precision_outer",    "avg. precision hits, outer;#eta;#phi",    xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_precision_extended = Book2D("_avg_hits_precision_extended", "avg. precision hits, extended;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);

    m_avg_hits_trigger_layer1 = Book2D("_avg_hits_trigger_layer1", "avg. trigger hits, layer 1;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_trigger_layer2 = Book2D("_avg_hits_trigger_layer2", "avg. trigger hits, layer 2;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_trigger_layer3 = Book2D("_avg_hits_trigger_layer3", "avg. trigger hits, layer 3;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_trigger_layer4 = Book2D("_avg_hits_trigger_layer4", "avg. trigger hits, layer 4;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);

    m_avg_hits_ibl = Book2D("_avg_hits_ibl", "avg. IBL hits;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_pix = Book2D("_avg_hits_pix", "avg. Pix hits;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_sct = Book2D("_avg_hits_sct", "avg. SCT hits;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_hits_trt = Book2D("_avg_hits_trt", "avg. TRT hits;#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);

    m_avg_ddpt_idme = Book2D("_avg_ddpt_idme", "avg. |pt_{ID} - pt_{ME}| / pt_{CB};#eta;#phi", xbins, xmin, xmax, ybins, ymin, ymax);
    m_avg_dptsignif = Book2D("_avg_dptsignif", "avg. momentum diff. signif.;#eta;#phi",      xbins, xmin, xmax, ybins, ymin, ymax);

  }
}

void RecoMuonPlots::fill(const xAOD::Muon& mu){
  //General Plots
  m_oAllPlots.fill(mu);
  m_oMuRecoInfoPlots.fill(mu);
  m_oMomentumPullPlots.fill(mu);

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

  if (!primaryTrk->summaryValue(hitval_numberOfPrecisionLayers,  xAOD::numberOfPrecisionLayers))  return; 
  if (!primaryTrk->summaryValue(hitval_numberOfPhiLayers,        xAOD::numberOfPhiLayers))        return; 
  if (!primaryTrk->summaryValue(hitval_numberOfTriggerEtaLayers, xAOD::numberOfTriggerEtaLayers)) return; 

  if (!mu.summaryValue(hitval_innerSmallHits,     xAOD::MuonSummaryType::innerSmallHits))    return; 
  if (!mu.summaryValue(hitval_innerLargeHits,     xAOD::MuonSummaryType::innerLargeHits))    return; 
  if (!mu.summaryValue(hitval_middleSmallHits,    xAOD::MuonSummaryType::middleSmallHits))   return; 
  if (!mu.summaryValue(hitval_middleLargeHits,    xAOD::MuonSummaryType::middleLargeHits))   return; 
  if (!mu.summaryValue(hitval_outerSmallHits,     xAOD::MuonSummaryType::outerSmallHits))    return; 
  if (!mu.summaryValue(hitval_outerLargeHits,     xAOD::MuonSummaryType::outerLargeHits))    return; 
  if (!mu.summaryValue(hitval_extendedSmallHits,  xAOD::MuonSummaryType::extendedSmallHits)) return; 
  if (!mu.summaryValue(hitval_extendedLargeHits,  xAOD::MuonSummaryType::extendedLargeHits)) return; 

  if (!mu.summaryValue(hitval_etaLayer1Hits,      xAOD::MuonSummaryType::etaLayer1Hits)) return; 
  if (!mu.summaryValue(hitval_etaLayer2Hits,      xAOD::MuonSummaryType::etaLayer2Hits)) return; 
  if (!mu.summaryValue(hitval_etaLayer3Hits,      xAOD::MuonSummaryType::etaLayer3Hits)) return; 
  if (!mu.summaryValue(hitval_etaLayer4Hits,      xAOD::MuonSummaryType::etaLayer4Hits)) return; 

  if (!mu.summaryValue(hitval_numberOfBLayerHits, xAOD::SummaryType::numberOfInnermostPixelLayerHits)) return;
  if (!mu.summaryValue(hitval_numberOfPixelHits,  xAOD::SummaryType::numberOfPixelHits))  return;
  if (!mu.summaryValue(hitval_numberOfSCTHits,    xAOD::SummaryType::numberOfSCTHits))    return;
  if (!mu.summaryValue(hitval_numberOfTRTHits,    xAOD::SummaryType::numberOfTRTHits))    return;

  m_eff_nPrec    -> Fill(mu.eta(), mu.phi(), hitval_numberOfPrecisionLayers);
  m_eff_nPhi     -> Fill(mu.eta(), mu.phi(), hitval_numberOfPhiLayers);
  m_eff_nTrigEta -> Fill(mu.eta(), mu.phi(), hitval_numberOfTriggerEtaLayers);

  m_avg_hits_precision_inner    -> Fill(mu.eta(), mu.phi(), hitval_innerSmallHits    + hitval_innerLargeHits);
  m_avg_hits_precision_middle   -> Fill(mu.eta(), mu.phi(), hitval_middleSmallHits   + hitval_middleLargeHits);
  m_avg_hits_precision_outer    -> Fill(mu.eta(), mu.phi(), hitval_outerSmallHits    + hitval_outerLargeHits);
  m_avg_hits_precision_extended -> Fill(mu.eta(), mu.phi(), hitval_extendedSmallHits + hitval_extendedLargeHits);

  m_avg_hits_trigger_layer1 -> Fill(mu.eta(), mu.phi(), hitval_etaLayer1Hits);
  m_avg_hits_trigger_layer2 -> Fill(mu.eta(), mu.phi(), hitval_etaLayer2Hits);
  m_avg_hits_trigger_layer3 -> Fill(mu.eta(), mu.phi(), hitval_etaLayer3Hits);
  m_avg_hits_trigger_layer4 -> Fill(mu.eta(), mu.phi(), hitval_etaLayer4Hits);

  m_avg_hits_ibl -> Fill(mu.eta(), mu.phi(), hitval_numberOfBLayerHits);
  m_avg_hits_pix -> Fill(mu.eta(), mu.phi(), hitval_numberOfPixelHits);
  m_avg_hits_sct -> Fill(mu.eta(), mu.phi(), hitval_numberOfSCTHits);
  m_avg_hits_trt -> Fill(mu.eta(), mu.phi(), hitval_numberOfTRTHits);

  if (primaryTrk && inDetTrk && msExtrapTrk) {
      qoverp_diff   = fabs(inDetTrk->qOverP() - msExtrapTrk->qOverP());
      qoverp_sigma  = sqrt(inDetTrk->definingParametersCovMatrix()(4,4) + msExtrapTrk->definingParametersCovMatrix()(4,4));
      qoverp_signif = (qoverp_sigma     > 0) ? (qoverp_diff / qoverp_sigma) : -999;
      ddpt_idme     = (primaryTrk->pt() > 0) ? fabs(inDetTrk->pt() - msExtrapTrk->pt()) / primaryTrk->pt() : -999;
      
      m_avg_ddpt_idme -> Fill(mu.eta(), mu.phi(), ddpt_idme);
      m_avg_dptsignif -> Fill(mu.eta(), mu.phi(), qoverp_signif);
  }

}
