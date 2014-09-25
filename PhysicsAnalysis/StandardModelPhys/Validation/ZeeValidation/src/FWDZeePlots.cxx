/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/FWDZeePlots.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  FWDZeePlots::FWDZeePlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir),
  m_sParticleType(sParticleType)
  {}     
  
  const std::string FWDZeePlots::cLevelLabel[nLevels] = {
    "Reco",
    "OQ", 
    "FWDLoose",
    "FWDTight"
  };

  void FWDZeePlots::initializePlots(){
    for (int i = 0; i < nLevels; i++) {
      z_mass[i] = Book1D("ZMass"+ cLevelLabel[i] , "Mass of Z;Mass (GeV);Events", 50, 66., 116.);
      z_pt[i] = Book1D("ZPt"+ cLevelLabel[i] , "P_{T} of Z;P_{T};Events", 120, 0., 120.);
      z_y[i] = Book1D("ZY"+ cLevelLabel[i] , "#eta of Z;#eta;Events", 50, -5., 5.);

      dr_electrons[i] = Book1D("DRElectrons"+ cLevelLabel[i] , " #DeltaR(electrons);Events", 50, 0., 6.);

      pt_central_electron[i] = Book1D("PtCentralElectron"+ cLevelLabel[i] , " P_{T}(electron);Events", 100, 20., 140.);
      eta_central_electron[i] = Book1D("EtaCentralElectron"+ cLevelLabel[i] , " #eta(electron);Events", 50, -2.5, 2.5);
      phi_central_electron[i] = Book1D("PhiCentralElectron"+ cLevelLabel[i] , " #varphi(electrons);Events", 50, -TMath::Pi(), TMath::Pi());

      pt_fwd_electron[i] = Book1D("PtFwdElectron"+ cLevelLabel[i] , " P_{T}(electron);Events", 100, 20., 140.);
      eta_fwd_electron[i] = Book1D("EtaFwdElectron"+ cLevelLabel[i] , " #eta(electron);Events", 50, -5.0, 5.0);
      phi_fwd_electron[i] = Book1D("PhiFwd_Electron"+ cLevelLabel[i] , " #varphi(electron);Events", 50, -TMath::Pi(), TMath::Pi());
    }
  }
  
  void FWDZeePlots::fillZPlots(TLorentzVector& z, int level){
    z_mass[level] -> Fill(z.M()/GeV);
    z_pt[level] -> Fill(z.Pt()/GeV);
    z_y[level] -> Fill(z.Rapidity());
  }

  void FWDZeePlots::fillElPlots(TLorentzVector& elec1, TLorentzVector& elec2, int level){
    dr_electrons[level] -> Fill( elec1.DeltaR(elec2) );

    pt_central_electron[level] -> Fill( elec1.Perp()/GeV  );
    pt_fwd_electron[level] -> Fill( elec2.Perp()/GeV  );

    eta_central_electron[level] -> Fill( elec1.Rapidity() );
    eta_fwd_electron[level] -> Fill( elec2.Rapidity() );

    phi_central_electron[level] -> Fill( elec1.Phi() );
    phi_fwd_electron[level] -> Fill( elec2.Phi() );
  }
 
}
