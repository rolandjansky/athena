/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/FWDZeePlots.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  FWDZeePlots::FWDZeePlots(PlotBase* pParent, std::string sDir, std::string sParticleType):
    PlotBase(pParent, sDir),
    m_sParticleType(sParticleType)
  {
    for (int i = 0; i < nLevels; i++) {
      h_z_mass[i] = NULL; 
      h_z_pt[i] = NULL; 
      h_z_y[i] = NULL; 
      h_dr_electrons[i] = NULL; 
      h_pt_central_electron[i] = NULL; 
      h_eta_central_electron[i] = NULL; 
      h_phi_central_electron[i] = NULL; 
      h_pt_fwd_electron[i] = NULL; 
      h_eta_fwd_electron[i] = NULL; 
      h_phi_fwd_electron[i] = NULL;  
    }
  }     
  
  const std::string FWDZeePlots::cLevelLabel[nLevels] = {
    "Reco",
    "OQ", 
    "FWDLoose",
    "FWDTight"
  };

  void FWDZeePlots::initializePlots(){
    for (int i = 0; i < nLevels; i++) {
      h_z_mass[i] = Book1D("ZMass"+ cLevelLabel[i] , "Mass of Z;Mass (GeV);Events", 50, 66., 116.);
      h_z_pt[i] = Book1D("ZPt"+ cLevelLabel[i] , "P_{T} of Z;P_{T};Events", 60, 0., 120.);
      h_z_y[i] = Book1D("ZY"+ cLevelLabel[i] , "#eta of Z;#eta;Events", 50, -5., 5.);

      h_dr_electrons[i] = Book1D("DRElectrons"+ cLevelLabel[i] , " #DeltaR(electrons);Events", 50, 0., 6.);

      h_pt_central_electron[i] = Book1D("PtCentralElectron"+ cLevelLabel[i] , " P_{T}(electron);Events", 60, 20., 140.);
      h_eta_central_electron[i] = Book1D("EtaCentralElectron"+ cLevelLabel[i] , " #eta(electron);Events", 50, -2.5, 2.5);
      h_phi_central_electron[i] = Book1D("PhiCentralElectron"+ cLevelLabel[i] , " #varphi(electrons);Events", 60, -TMath::Pi(), TMath::Pi());

      h_pt_fwd_electron[i] = Book1D("PtFwdElectron"+ cLevelLabel[i] , " P_{T}(electron);Events", 60, 20., 140.);
      h_eta_fwd_electron[i] = Book1D("EtaFwdElectron"+ cLevelLabel[i] , " #eta(electron);Events", 50, -5.0, 5.0);
      h_phi_fwd_electron[i] = Book1D("PhiFwd_Electron"+ cLevelLabel[i] , " #varphi(electron);Events", 60, -TMath::Pi(), TMath::Pi());
    }
  }
  
  void FWDZeePlots::fillZPlots(TLorentzVector& z, int level){
    h_z_mass[level] -> Fill(z.M()*(1./GeV));
    h_z_pt[level] -> Fill(z.Pt()*(1./GeV));
    h_z_y[level] -> Fill(z.Rapidity());
  }

  void FWDZeePlots::fillElPlots(TLorentzVector& elec1, TLorentzVector& elec2, int level){
    h_dr_electrons[level] -> Fill( elec1.DeltaR(elec2) );

    h_pt_central_electron[level] -> Fill( elec1.Perp()*(1./GeV)  );
    h_pt_fwd_electron[level] -> Fill( elec2.Perp()*(1./GeV)  );

    h_eta_central_electron[level] -> Fill( elec1.Rapidity() );
    h_eta_fwd_electron[level] -> Fill( elec2.Rapidity() );

    h_phi_central_electron[level] -> Fill( elec1.Phi() );
    h_phi_fwd_electron[level] -> Fill( elec2.Phi() );
  }
 
}
