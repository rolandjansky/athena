/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/TrueElectronsPlots.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  TrueElectronsPlots::TrueElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir),
  m_sParticleType(sParticleType)
  {}     

  const std::string TrueElectronsPlots::cLevelLabel[nLevels] = {
    "True",
    "Assoc", 
    "Matched",
    "OQ",
    "Loose",
    "Medium",
    "Tight"
  }; 
  
  void TrueElectronsPlots::initializePlots(){
   TH1::SetDefaultSumw2(kTRUE);  

    for (int i = 0; i < nLevels; i++) {
      electron_pt[i]  = Book1D("Pt" + cLevelLabel[i], "P_{T} of "+ m_sParticleType + "; E_{T} (GeV);Events", 100, 20., 140.);
      electron_eta[i] = Book1D("Eta" + cLevelLabel[i], "#eta of " + m_sParticleType + ";#eta;Events", 100, -2.5, 2.5);
      electron_phi[i] = Book1D("Phi" + cLevelLabel[i], "#varphi of " + m_sParticleType + ";#varphi;Events", 100, -TMath::Pi(), TMath::Pi());
      electron_etavsphi[i] = Book2D("EtaVSPhi" + cLevelLabel[i], "#eta vs #varphi of " + m_sParticleType + ";#eta;#varphi", 100, -2.5, 2.5, 100, -TMath::Pi(), TMath::Pi());
    }

   for (int i = 0; i < nLevels-1; i++) {
      electron_eff_pt[i]  = Book1D("PtEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs P_{T} of "+ m_sParticleType + "; E_{T} (GeV);" + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, 20., 140.);
      electron_eff_eta[i] = Book1D("EtaEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #eta of " + m_sParticleType + ";#eta;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, -2.5, 2.5);
      electron_eff_phi[i] = Book1D("PhiEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #varphi of " + m_sParticleType + ";#varphi;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, -TMath::Pi(), TMath::Pi());
    }
  
  }
  
  void TrueElectronsPlots::fill(const xAOD::IParticle* part, int level){
    electron_eta[level] -> Fill(part -> eta());
    electron_etavsphi[level] -> Fill(part -> eta(), part -> phi());
  }

  void TrueElectronsPlots::fillinAcc(const xAOD::IParticle* part, int level){
    electron_pt[level] -> Fill(part -> pt()/GeV);
    electron_phi[level] -> Fill(part -> phi());
  }

  void TrueElectronsPlots::finalize() {

    for (int i = 0; i < nLevels-1; i++) {
      electron_eff_pt[i] -> Divide(electron_pt[i+1], electron_pt[0], 1, 1, "B");
      electron_eff_eta[i] -> Divide(electron_eta[i+1], electron_eta[0], 1, 1, "B");
      electron_eff_phi[i] -> Divide(electron_phi[i+1], electron_phi[0], 1, 1, "B");
      }
  }
 
}
