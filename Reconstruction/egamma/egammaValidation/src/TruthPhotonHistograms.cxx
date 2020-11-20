/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthPhotonHistograms.h"

using namespace egammaMonitoring;

#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AnaToolHandle.h"

StatusCode TruthPhotonHistograms::initializePlots() {

  ATH_CHECK(ParticleHistograms::initializePlots());

  histoMap["convRadius"] = new TH1D(Form("%s_%s",m_name.c_str(),"convRadius"), ";Conversion Radius [mm]; Conversion Radius Events", 14, m_cR_bins);
  histoMap["convRadiusTrueVsReco"] = new TH1D(Form("%s_%s",m_name.c_str(),"convRadiusTrueVsReco"), ";R^{reco}_{conv. vtx} - R^{true}_{conv. vtx} [mm]; Events", 100, -200, 200);

  histoMap["resolution_e"] = new TH1D(Form("%s_%s", m_name.c_str(), "resolution_e"), "; E_{reco} - E_{true} / E_{truth}; Events", 40, -0.2, 0.2);
  histoMap["resolution_pT"] = new TH1D(Form("%s_%s", m_name.c_str(), "resolution_pT"), "; p_{T}_{reco} - p_{T}_{true} / p_{T}_{truth}; Events", 40, -0.2, 0.2);
  histoMap["resolution_eta"] = new TH1D(Form("%s_%s", m_name.c_str(), "resolution_eta"), "; #eta_{reco} - #eta_{true}; Events", 20, -0.05, 0.05);
  histoMap["resolution_phi"] = new TH1D(Form("%s_%s", m_name.c_str(), "resolution_phi"), "; #phi_{reco} - #phi_{true}; Events", 20, -0.05, 0.05);

  histo2DMap["resolution_e_vs_pT"] = new TH2D(Form("%s_%s",m_name.c_str(),"resolution_e_vs_pT"), ";p_{T};E_{reco} - E_{true} / E_{truth}", 40, 0, 200, 160, -0.2, 0.2); 
  histo2DMap["resolution_e_vs_eta"] = new TH2D(Form("%s_%s",m_name.c_str(),"resolution_e_vs_eta"), ";#eta;E_{reco} - E_{true} / E_{truth}", 20, 0, 3, 160, -0.2, 0.2); 

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"convRadius", histoMap["convRadius"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"convRadiusTrueVsReco", histoMap["convRadiusTrueVsReco"]));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"resolution_e", histoMap["resolution_e"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"resolution_pT", histoMap["resolution_pT"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"resolution_eta", histoMap["resolution_eta"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"resolution_phi", histoMap["resolution_phi"]));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"resolution_e_vs_pT", histo2DMap["resolution_e_vs_pT"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"resolution_e_vs_eta", histo2DMap["resolution_e_vs_eta"]));

  return StatusCode::SUCCESS;

}

void TruthPhotonHistograms::fill(const xAOD::IParticle& phrec) {

  float trueR = -999;
  float res_e = -999;
  float res_pT = -999;
  float res_eta = -999;
  float res_phi = -999;
  float recoR = -999;

  ParticleHistograms::fill(phrec);

  const xAOD::TruthParticle *truth  = xAOD::TruthHelpers::getTruthParticle(phrec);

  if (truth) {
    if (truth->pdgId() == 22 && truth->hasDecayVtx()) {

      float x = truth->decayVtx()->x();
      float y = truth->decayVtx()->y();
      trueR = std::sqrt( x*x + y*y );

    }
  }

  histoMap["convRadius"]->Fill(trueR);

  // access reco photon from the xAOD::TruthParticle (can't use the IParticle* here)
  auto truthParticle = dynamic_cast<const xAOD::TruthParticle*>(&phrec);
  if (truthParticle) {
    const xAOD::Photon *photon = xAOD::EgammaHelpers::getRecoPhoton(truthParticle);

    if (photon) {
      res_e = (photon->e() - truth->e())/truth->e();
      res_pT = (photon->pt() - truth->pt())/truth->pt();
      res_eta = photon->eta() - truth->eta();
      res_phi = photon->phi() - truth->phi();
      recoR = xAOD::EgammaHelpers::conversionRadius(photon);

    }
  }

  histoMap["convRadiusTrueVsReco"]->Fill(recoR - trueR);

  histoMap["resolution_e"]->Fill(res_e);
  histoMap["resolution_pT"]->Fill(res_pT);
  histoMap["resolution_eta"]->Fill(res_eta);
  histoMap["resolution_phi"]->Fill(res_phi);

  histo2DMap["resolution_e_vs_pT"]->Fill(phrec.pt()/1000., res_e);
  histo2DMap["resolution_e_vs_eta"]->Fill(std::abs(phrec.eta()), res_e);


} // fill
