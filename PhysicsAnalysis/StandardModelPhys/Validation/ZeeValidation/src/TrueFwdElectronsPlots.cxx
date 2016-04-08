/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/TrueFwdElectronsPlots.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  TrueFwdElectronsPlots::TrueFwdElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):
    PlotBase(pParent, sDir),
    m_sParticleType(sParticleType),
    h_fwd_e_response_vs_e(NULL),
    h_fwd_e_response_vs_eta(NULL),
    h_fwd_e_response_vs_abseta(NULL),
    h_fwd_e_response_vs_phi(NULL),
    h_fwd_dr_electron(NULL),
    h_fwd_dphi_electron(NULL),
    h_fwd_deta_electron(NULL)
  {
    for (int i = 0; i < nLevels; i++) { 
      h_true_fwd_electron_pt[i] = NULL;
      h_true_fwd_electron_eta[i] = NULL; 
      h_true_fwd_electron_abseta[i] = NULL; 
      h_true_fwd_electron_phi[i] = NULL;
    }
    for (int i = 0; i < nLevels-1; i++) {
      h_true_fwd_electron_eff_pt[i] = NULL;
      h_true_fwd_electron_eff_eta[i] = NULL; 
      h_true_fwd_electron_eff_abseta[i] = NULL;
      h_true_fwd_electron_eff_phi[i] = NULL; 
    } 
  }     
  
 const std::string TrueFwdElectronsPlots::cLevelLabel[nLevels] = {
    "True", 
    "Matched",
    "Loose",
    "Tight"
  }; 
  //-------------------------------------------------
  void TrueFwdElectronsPlots::initializePlots(){
    TH1::SetDefaultSumw2(kTRUE); 

    for (int i = 0; i < nLevels; i++) { 
      h_true_fwd_electron_pt[i]      = Book1D("PtFwd" + cLevelLabel[i], "P_{T} of "+ m_sParticleType +"; E_{T} (GeV) ;Events", 60, 20., 140.);
      h_true_fwd_electron_eta[i]     = Book1D("EtaFwd" + cLevelLabel[i], "#eta of " + m_sParticleType +";#eta; Events ", 50, -5., 5.);
      h_true_fwd_electron_abseta[i]  = Book1D("EtaAbsFwd" + cLevelLabel[i], "|#eta| of " + m_sParticleType +";|#eta|; Events ", 25, 2.5, 5.);
      h_true_fwd_electron_phi[i]     = Book1D("PhiFwd" + cLevelLabel[i], "#varphi of " + m_sParticleType +";#varphi;Events ", 60, -TMath::Pi(), TMath::Pi());
    }

    for (int i = 0; i < nLevels-1; i++) {
      h_true_fwd_electron_eff_pt[i]     = BookTProfile("PtFwdEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs P_{T} of "+ m_sParticleType + "; E_{T} (GeV);" + cLevelLabel[i+1] + "/" + cLevelLabel[0], 60, 20., 140.);
      h_true_fwd_electron_eff_eta[i]    = BookTProfile("EtaFwdEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #eta of " + m_sParticleType + ";#eta;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 50, -5., 5.);
      h_true_fwd_electron_eff_abseta[i] = BookTProfile("EtaAbsFwdEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs |#eta| of " + m_sParticleType + ";|#eta;|"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 25, 2.5, 5.);
      h_true_fwd_electron_eff_phi[i]    = BookTProfile("PhiFwdEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #varphi of " + m_sParticleType + ";#varphi;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 60, -TMath::Pi(), TMath::Pi());
    }

    h_fwd_e_response_vs_e           = BookTProfile("EnergyResponseVsEnergy", "Energy response vs energy; E^{true}; E^{reco}-E^{true}/E^{true}", 25, 0., 1000);
    h_fwd_e_response_vs_eta         = BookTProfile("EnergyResponseVsEta", "Energy response vs #eta; #eta; E^{reco}-E^{true}/E^{true}", 50, -5., 5.);
    h_fwd_e_response_vs_abseta      = BookTProfile("EnergyResponseVsAbsEta", "Energy response vs |#eta|; |#eta|; E^{reco}-E^{true}/E^{true}", 50, 2.5, 5.);
    h_fwd_e_response_vs_phi         = BookTProfile("EnergyResponseVsPhi", "Energy response vs #varphi; #varphi; E^{reco}-E^{true}/E^{true}", 30, -TMath::Pi(), TMath::Pi());
 
    h_fwd_dr_electron     = Book1D("DeltaRFwdElectron", "#DeltaR(truth particle, electron); #DeltaR; Events", 30, 0., 0.2);
    h_fwd_dphi_electron   = Book1D("DeltaPhiFwdElectron", "#Delta #phi(truth particle, electron); #Delta #phi; Events", 20, -0.06, 0.06);
    h_fwd_deta_electron   = Book1D("DeltaEtaFwdElectron", "#Delta #eta(truth particle, electron); #Delta #eta; Events", 20, -0.06, 0.06);

  }
  //-------------------------------------------------
  void TrueFwdElectronsPlots::fill(const xAOD::IParticle* part, int level){

    h_true_fwd_electron_pt[level]     -> Fill(part -> pt()*(1./GeV));
    h_true_fwd_electron_eta[level]    -> Fill(part -> eta());
    h_true_fwd_electron_abseta[level] -> Fill(TMath::Abs(part -> eta()));
    h_true_fwd_electron_phi[level]    -> Fill(part -> phi());

  }
  //-------------------------------------------------
  void TrueFwdElectronsPlots::fillResponse(const xAOD::IParticle* part, const xAOD::Electron* electron) {
    
    h_fwd_e_response_vs_e -> Fill( part -> e()*(1./GeV), (electron -> e() - part -> e())/part -> e() );
    h_fwd_e_response_vs_eta -> Fill( part -> eta(), (electron -> e() - part -> e())/part -> e() );   
    h_fwd_e_response_vs_abseta -> Fill( TMath::Abs(part -> eta()), (electron -> e() - part -> e())/part -> e() );   
    h_fwd_e_response_vs_phi -> Fill( part -> phi(), (electron -> e() - part -> e())/part -> e() ); 

    h_fwd_dr_electron -> Fill( part -> p4().DeltaR(electron -> p4()));
    h_fwd_dphi_electron -> Fill( part -> p4().DeltaPhi(electron -> p4()));
    h_fwd_deta_electron -> Fill( electron -> eta() - part -> eta() );
  }

  //-------------------------------------------------
  void TrueFwdElectronsPlots::makeEfficiencyPlot(TH1* hDenom, TH1* hNom, TProfile* hEff){

    if (hDenom->GetNbinsX() != hNom->GetNbinsX()) return;
    else{
      for (int bin_i = 1; bin_i <= hDenom -> GetNbinsX(); ++bin_i){
	if(hDenom -> GetBinContent(bin_i) == 0) continue;
	double binContent = hNom -> GetBinContent(bin_i) / hDenom -> GetBinContent(bin_i);
	double binCenter = hNom -> GetBinCenter(bin_i);

	hEff -> Fill(binCenter, binContent);
      }
    }

  }
  //-------------------------------------------------
  void TrueFwdElectronsPlots::finalizePlots() {

    for (int i = 0; i < nLevels-1; i++) {
      makeEfficiencyPlot( h_true_fwd_electron_pt[0], h_true_fwd_electron_pt[i+1], h_true_fwd_electron_eff_pt[i]);
      makeEfficiencyPlot( h_true_fwd_electron_eta[0], h_true_fwd_electron_eta[i+1], h_true_fwd_electron_eff_eta[i]);
      makeEfficiencyPlot( h_true_fwd_electron_abseta[0], h_true_fwd_electron_abseta[i+1], h_true_fwd_electron_eff_abseta[i]);
      makeEfficiencyPlot( h_true_fwd_electron_phi[0], h_true_fwd_electron_phi[i+1], h_true_fwd_electron_eff_phi[i]);
    }

  }
}
