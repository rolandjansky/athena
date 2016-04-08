/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/TrueElectronsPlots.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  TrueElectronsPlots::TrueElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):
    PlotBase(pParent, sDir),
    m_sParticleType(sParticleType),
    h_e_response_vs_e(NULL),
    h_e_cluster_response_vs_e(NULL),  
    h_e_response_vs_eta(NULL),
    h_e_cluster_response_vs_eta(NULL) ,
    h_e_response_vs_phi(NULL),
    h_e_cluster_response_vs_phi(NULL) ,
    h_dr_electron(NULL),
    h_dphi_electron(NULL),
    h_deta_electron(NULL),
    h_dr_photon(NULL)
  {
    for (int i = 0; i < nLevels; i++) {
      h_electron_pt[i] = NULL; 
      h_electron_eta[i] = NULL;
      h_electron_phi[i] = NULL;
    }
    for (int i = 0; i < nLevels-1; i++) {
      h_electron_eff_pt[i] = NULL; 
      h_electron_eff_eta[i] = NULL;
      h_electron_eff_phi[i] = NULL;
    }
  }     
  //-------------------------------------------------
  const std::string TrueElectronsPlots::cLevelLabel[nLevels] = {
    "True",
    "Assoc", 
    "Matched",
    "OQ",
    "Loose",
    "Medium",
    "Tight",
    "LHLoose",
    "LHMedium",
    "LHTight"
  }; 
  //------------------------------------------------- 
  void TrueElectronsPlots::initializePlots(){
    TH1::SetDefaultSumw2(kTRUE);  
    TProfile::SetDefaultSumw2(kTRUE);  

    for (int i = 0; i < nLevels; i++) {
      h_electron_pt[i]  = Book1D("Pt" + cLevelLabel[i], "P_{T} of "+ m_sParticleType + "; P_{T} (GeV);Events", 60, 20., 140.);
      h_electron_eta[i] = Book1D("Eta" + cLevelLabel[i], "#eta of " + m_sParticleType + ";#eta;Events", 50, -2.5, 2.5);
      h_electron_phi[i] = Book1D("Phi" + cLevelLabel[i], "#varphi of " + m_sParticleType + ";#varphi;Events", 60, -TMath::Pi(), TMath::Pi());
    }

    for (int i = 0; i < nLevels-1; i++) {
      h_electron_eff_pt[i]  = BookTProfile("PtEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs P_{T} of "+ m_sParticleType + "; E_{T} (GeV);" + cLevelLabel[i+1] + "/" + cLevelLabel[0], 60, 20., 140.);
      h_electron_eff_eta[i] = BookTProfile("EtaEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #eta of " + m_sParticleType + ";#eta;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 50, -2.5, 2.5);
      h_electron_eff_phi[i] = BookTProfile("PhiEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #varphi of " + m_sParticleType + ";#varphi;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 60, -TMath::Pi(), TMath::Pi());
    }

    h_e_response_vs_e              = BookTProfile("EnergyResponseVsEnergy", "Energy response vs energy; E^{true}; E^{reco}-E^{true}/E^{true}", 20, 0., 200);
    h_e_response_vs_eta            = BookTProfile("EnergyResponseVsEta", "Energy response vs #eta; #eta; E^{reco}-E^{true}/E^{true}", 30, -3., 3.); 
    h_e_response_vs_phi            = BookTProfile("EnergyResponseVsPhi", "Energy response vs #varphi; #varphi; E^{reco}-E^{true}/E^{true}", 30, -TMath::Pi(), TMath::Pi());  
    h_e_cluster_response_vs_e      = BookTProfile("EnergyClusterResponseVsEnergy", "Energy of cluster response vs energy; E^{true}; E^{reco}-E^{true}/E^{true}", 20, 0., 200);
    h_e_cluster_response_vs_eta    = BookTProfile("EnergyClusterResponseVsEta", "Energy of cluster response vs #eta; #eta; E^{reco}-E^{true}/E^{true}", 30, -3., 3.);  
    h_e_cluster_response_vs_phi    = BookTProfile("EnergyClusterResponseVsPhi", "Energy of cluster response vs #varphi; #varphi; E^{reco}-E^{true}/E^{true}", 30, -TMath::Pi(), TMath::Pi());  
  
    h_dr_electron     = Book1D("DeltaRElectron", "#DeltaR(truth particle, electron); #DeltaR; Events", 30, 0., 0.2);
    h_dphi_electron   = Book1D("DeltaPhiElectron", "#Delta #phi(truth particle, electron); #Delta #phi; Events", 20, -0.01, 0.01);
    h_deta_electron   = Book1D("DeltaEtaElectron", "#Delta #eta(truth particle, electron); #Delta #eta; Events", 20, -0.01, 0.01);

    h_dr_photon       = Book1D("DeltaRPhoton", "#DeltaR(truth particle, photon); #DeltaR; Events", 30, 0., 0.2);

  }
  //-------------------------------------------------  
  void TrueElectronsPlots::fill(const xAOD::IParticle* part, int level){
    h_electron_eta[level] -> Fill(part -> eta());
  }
  //-------------------------------------------------
  void TrueElectronsPlots::fillinAcc(const xAOD::IParticle* part, int level){
    h_electron_pt[level] -> Fill(part -> pt()*(1./GeV));
    h_electron_phi[level] -> Fill(part -> phi());
  }

  //-------------------------------------------------
  void TrueElectronsPlots::fillResponse(const xAOD::IParticle* part, const xAOD::Electron* electron) {
    
    h_e_response_vs_e -> Fill( part -> e()*(1./GeV), (electron -> e() - part -> e())/part -> e() );
    h_e_response_vs_eta -> Fill( part -> eta(), (electron -> e() - part -> e())/part -> e() );   
    h_e_response_vs_phi -> Fill( part -> phi(), (electron -> e() - part -> e())/part -> e() );   
 
    h_dr_electron   -> Fill( part -> p4().DeltaR(electron -> p4()));
    h_dphi_electron -> Fill( part -> p4().DeltaPhi(electron -> p4()));
    h_deta_electron -> Fill( electron -> eta() - part -> eta() );

  }
  //-------------------------------------------------
  void TrueElectronsPlots::fillResponseCluster(const xAOD::IParticle* part, const xAOD::CaloCluster* cluster) {
    
    h_e_cluster_response_vs_e -> Fill( part -> e()*(1./GeV), (cluster -> e() - part -> e())/part -> e() );  
    h_e_cluster_response_vs_eta -> Fill( part -> eta(), (cluster -> e() - part -> e())/part -> e() );    
    h_e_cluster_response_vs_phi -> Fill( part -> phi(), (cluster -> e() - part -> e())/part -> e() );  
  
  }
  //-------------------------------------------------
  void TrueElectronsPlots::makeEfficiencyPlot(TH1* hDenom, TH1* hNom, TProfile* hEff){

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
  void TrueElectronsPlots::finalizePlots() {

    for (int i = 0; i < nLevels-1; i++) {
      makeEfficiencyPlot( h_electron_pt[0], h_electron_pt[i+1], h_electron_eff_pt[i]);
      makeEfficiencyPlot( h_electron_phi[0], h_electron_phi[i+1], h_electron_eff_phi[i]);
      makeEfficiencyPlot( h_electron_eta[0], h_electron_eta[i+1], h_electron_eff_eta[i]);
    }

  }
}
