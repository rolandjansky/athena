/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/ZeePlots.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  ZeePlots::ZeePlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir),
										     m_sParticleType(sParticleType)
  {}     
  
  const std::string ZeePlots::cLevelLabel[nLevels] = {
    "Reco_OS",
    "OQ_OS", 
    "Loose_OS",
    "Medium_OS",
    "Tight_OS",
    "Reco_SS",
    "OQ_SS", 
    "Loose_SS",
    "Medium_SS",
    "Tight_SS"
  };
  
  void ZeePlots::initializePlots(){

    n_el = Book1D("NElectrons", "Number of electrons; Number ;Events", 10, -0.5, 9.5);

    for (int i = 0; i < nLevels; i++) {

      z_mass[i] = Book1D("ZMass"+ cLevelLabel[i], "Mass of Z; Mass (GeV) ;Events", 50, 66., 116.);
      z_pt[i] = Book1D("ZPt"+ cLevelLabel[i], "P_{T} of Z; P_{T} ;Events", 120, 0., 120.);
      z_y[i] = Book1D("ZY"+ cLevelLabel[i], "#eta of Z; #eta ;Events", 50, -5., 5.);

      dr_electrons[i] = Book1D("DRElectrons"+ cLevelLabel[i], " #DeltaR(electrons) ;Events", 50, 0., 6.);

      ptclust_electron1[i] = Book1D("PtClustElectron1"+ cLevelLabel[i], " P_{T}(el cluster_{1}) ;Events", 100, 20., 140.);
      ptclust_electron2[i] = Book1D("PtClustElectron2"+ cLevelLabel[i], " P_{T}(el cluster_{2}) ;Events", 100, 20., 140.);
      etaclust_electron1[i] = Book1D("EtaClustElectron1"+ cLevelLabel[i], " #eta(el cluster_{1}) ;Events", 50, -2.5, 2.5);
      etaclust_electron2[i] = Book1D("EtaClustElectron2"+ cLevelLabel[i], " #eta(el cluster_{2}) ;Events", 50, -2.5, 2.5);

      ptclust_electrons[i] = Book1D("PtClust_Electrons"+ cLevelLabel[i], " P_{T}(el clusters) ;Events", 100, 20., 140.);
      etaclust_electrons[i] = Book1D("EtaClustElectrons"+ cLevelLabel[i], " #eta(el clusters) ;Events", 50, -2.5, 2.5);
      pt_electrons[i] = Book1D("PtElectrons"+ cLevelLabel[i], " P_{T}(electrons) ;Events", 100, 20., 140.);
      eta_electrons[i] = Book1D("EtaElectrons"+ cLevelLabel[i], " #eta(electrons) ;Events", 50, -2.5, 2.5);
      phi_electrons[i] = Book1D("PhiElectrons"+ cLevelLabel[i], " #varphi(electrons) ;Events", 50, -TMath::Pi(), TMath::Pi());
      eoverp_electrons[i] = Book1D("EoverPElectrons"+ cLevelLabel[i], " E/p ;Events", 100, 0., 10.);
      eoverpq_electrons[i] = Book1D("EoverPQElectrons"+ cLevelLabel[i], "E/p*q ;Events", 100, -10., 10.);
    }

  }

  void ZeePlots::fillZPlots(TLorentzVector& z, int level){
    z_mass[level] -> Fill(z.M()/GeV);
    z_pt[level] -> Fill(z.Pt()/GeV);
    z_y[level] -> Fill(z.Rapidity());
  }

  void ZeePlots::fillElectronPlots( TLorentzVector& elec1, TLorentzVector& elec2, TLorentzVector& cluster1, TLorentzVector& cluster2, TLorentzVector& track1, TLorentzVector& track2, int charge1, int charge2, int level ){
   
    dr_electrons[level] -> Fill( elec1.DeltaR(elec2) );

    ptclust_electron1[level] -> Fill( cluster1.Perp()/GeV );
    ptclust_electron2[level] -> Fill( cluster2.Perp()/GeV );

    etaclust_electron1[level] -> Fill( cluster1.Rapidity() );
    etaclust_electron2[level] -> Fill( cluster2.Rapidity() );

    ptclust_electrons[level] -> Fill( cluster1.Perp()/GeV );
    ptclust_electrons[level] -> Fill( cluster2.Perp()/GeV );

    etaclust_electrons[level] -> Fill( cluster1.Rapidity() );
    etaclust_electrons[level] -> Fill( cluster2.Rapidity() );

    pt_electrons[level] -> Fill( elec1.Perp()/GeV );
    pt_electrons[level] -> Fill( elec2.Perp()/GeV );

    eta_electrons[level] -> Fill( elec1.Rapidity() );
    eta_electrons[level] -> Fill( elec2.Rapidity() );

    phi_electrons[level] -> Fill( elec1.Phi() );
    phi_electrons[level] -> Fill( elec2.Phi() );

    eoverp_electrons[level] ->Fill( cluster1.E()/track1.E() );
    eoverp_electrons[level] ->Fill( cluster2.E()/track2.E() );  

    eoverpq_electrons[level] ->Fill( cluster1.E()/track1.E()*charge1 );
    eoverpq_electrons[level] ->Fill( cluster2.E()/track2.E()*charge2 );  
  }

  void ZeePlots::fillGenPlots( int nel ){
   n_el -> Fill( nel );
  }
  
}
