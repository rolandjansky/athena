/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/TrueFwdElectronsPlots.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  TrueFwdElectronsPlots::TrueFwdElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir),
  m_sParticleType(sParticleType)
  {}     
  
 const std::string TrueFwdElectronsPlots::cLevelLabel[nLevels] = {
    "True", 
    "Matched",
    "Loose",
    "Tight"
  }; 

  void TrueFwdElectronsPlots::initializePlots(){
    TH1::SetDefaultSumw2(kTRUE); 

    for (int i = 0; i < nLevels; i++) {
      true_fwd_electron_pt[i]  = Book1D("PtFwd" + cLevelLabel[i], "P_{T} of "+ m_sParticleType +"; E_{T} (GeV) ;Events", 100, 20., 140.);
      true_fwd_electron_eta[i] = Book1D("EtaFwd" + cLevelLabel[i], "#eta of " + m_sParticleType +";#eta; Events ", 100, -5., 5.);
      true_fwd_electron_phi[i] = Book1D("PhiFwd" + cLevelLabel[i], "#varphi of " + m_sParticleType +";#varphi;Events ", 100, -TMath::Pi(), TMath::Pi());
    }

    for (int i = 0; i < nLevels-1; i++) {
      true_fwd_electron_eff_pt[i]  = Book1D("PtFwdEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs P_{T} of "+ m_sParticleType + "; E_{T} (GeV);" + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, 20., 140.);
      true_fwd_electron_eff_eta[i] = Book1D("EtaFwdEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #eta of " + m_sParticleType + ";#eta;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, -2.5, 2.5);
      true_fwd_electron_eff_phi[i] = Book1D("PhiFwdEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #varphi of " + m_sParticleType + ";#varphi;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, -TMath::Pi(), TMath::Pi());
    }


  }
  
  void TrueFwdElectronsPlots::fill(const xAOD::IParticle* part, int level){
    true_fwd_electron_pt[level] -> Fill(part->pt()/GeV);
    true_fwd_electron_eta[level] -> Fill(part -> eta());
    true_fwd_electron_phi[level] -> Fill(part -> phi());
  }

 void TrueFwdElectronsPlots::finalize() {

    for (int i = 0; i < nLevels-1; i++) {
      true_fwd_electron_eff_pt[i] -> Divide(true_fwd_electron_pt[i+1], true_fwd_electron_pt[0], 1, 1, "B");
      true_fwd_electron_eff_eta[i] -> Divide(true_fwd_electron_eta[i+1], true_fwd_electron_eta[0], 1, 1, "B");
      true_fwd_electron_eff_phi[i] -> Divide(true_fwd_electron_phi[i+1], true_fwd_electron_phi[0], 1, 1, "B");
      }
  }
   
}
