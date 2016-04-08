/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/ZeePlots.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  ZeePlots::ZeePlots(PlotBase* pParent, std::string sDir, std::string sParticleType):
    PlotBase(pParent, sDir),
    m_sParticleType(sParticleType),
    h_n_el(NULL)
  {
    for (int i = 0; i < nLevels; i++) {
      h_z_mass[i] = NULL; 
      h_z_pt[i] = NULL; 
      h_z_y[i] = NULL; 
      h_dr_electrons[i] = NULL; 
      h_ptclust_electron1[i] = NULL; 
      h_ptclust_electron2[i] = NULL; 
      h_etaclust_electron1[i] = NULL; 
      h_etaclust_electron2[i] = NULL; 
      h_ptclust_electrons[i] = NULL;  
      h_etaclust_electrons[i] = NULL;  
      h_pt_electrons[i] = NULL; 
      h_eta_electrons[i] = NULL; 
      h_phi_electrons[i] = NULL; 
      h_eoverp_electrons[i] = NULL; 
      h_eoverpq_electrons[i] = NULL;
    }
  }     
  
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

    h_n_el = Book1D("NElectrons", "Number of electrons; Number ;Events", 10, -0.5, 9.5);

    for (int i = 0; i < nLevels; i++) {

      h_z_mass[i] = Book1D("ZMass"+ cLevelLabel[i], "Mass of Z; Mass (GeV) ;Events", 50, 66., 116.);
      h_z_pt[i] = Book1D("ZPt"+ cLevelLabel[i], "P_{T} of Z; P_{T} ;Events", 60, 0., 120.);
      h_z_y[i] = Book1D("ZY"+ cLevelLabel[i], "#eta of Z; #eta ;Events", 100, -5., 5.);

      h_dr_electrons[i] = Book1D("DRElectrons"+ cLevelLabel[i], " #DeltaR(electrons) ;Events", 50, 0., 6.);

      h_ptclust_electron1[i] = Book1D("PtClustElectron1"+ cLevelLabel[i], " P_{T}(el cluster_{1}) ;Events", 60, 20., 140.);
      h_ptclust_electron2[i] = Book1D("PtClustElectron2"+ cLevelLabel[i], " P_{T}(el cluster_{2}) ;Events", 60, 20., 140.);
      h_etaclust_electron1[i] = Book1D("EtaClustElectron1"+ cLevelLabel[i], " #eta(el cluster_{1}) ;Events", 50, -2.5, 2.5);
      h_etaclust_electron2[i] = Book1D("EtaClustElectron2"+ cLevelLabel[i], " #eta(el cluster_{2}) ;Events", 50, -2.5, 2.5);

      h_ptclust_electrons[i] = Book1D("PtClust_Electrons"+ cLevelLabel[i], " P_{T}(el clusters) ;Events", 60, 20., 140.);
      h_etaclust_electrons[i] = Book1D("EtaClustElectrons"+ cLevelLabel[i], " #eta(el clusters) ;Events", 50, -2.5, 2.5);
      h_pt_electrons[i] = Book1D("PtElectrons"+ cLevelLabel[i], " P_{T}(electrons) ;Events", 60, 20., 140.);
      h_eta_electrons[i] = Book1D("EtaElectrons"+ cLevelLabel[i], " #eta(electrons) ;Events", 50, -2.5, 2.5);
      h_phi_electrons[i] = Book1D("PhiElectrons"+ cLevelLabel[i], " #varphi(electrons) ;Events", 60, -TMath::Pi(), TMath::Pi());
      h_eoverp_electrons[i] = Book1D("EoverPElectrons"+ cLevelLabel[i], " E/p ;Events", 50, 0., 10.);
      h_eoverpq_electrons[i] = Book1D("EoverPQElectrons"+ cLevelLabel[i], "E/p*q ;Events", 50, -10., 10.);
    }

  }

  void ZeePlots::fillZPlots(TLorentzVector& z, int level){
    h_z_mass[level] -> Fill(z.M()*(1./GeV));
    h_z_pt[level] -> Fill(z.Pt()*(1./GeV));
    h_z_y[level] -> Fill(z.Rapidity());
  }

  void ZeePlots::fillElectronPlots( TLorentzVector& elec1, TLorentzVector& elec2, TLorentzVector& cluster1, TLorentzVector& cluster2, TLorentzVector& track1, TLorentzVector& track2, int charge1, int charge2, int level ){
   
    h_dr_electrons[level] -> Fill( elec1.DeltaR(elec2) );

    h_ptclust_electron1[level] -> Fill( cluster1.Perp()*(1./GeV) );
    h_ptclust_electron2[level] -> Fill( cluster2.Perp()*(1./GeV) );

    h_etaclust_electron1[level] -> Fill( cluster1.Rapidity() );
    h_etaclust_electron2[level] -> Fill( cluster2.Rapidity() );

    h_ptclust_electrons[level] -> Fill( cluster1.Perp()*(1./GeV) );
    h_ptclust_electrons[level] -> Fill( cluster2.Perp()*(1./GeV) );

    h_etaclust_electrons[level] -> Fill( cluster1.Rapidity() );
    h_etaclust_electrons[level] -> Fill( cluster2.Rapidity() );

    h_pt_electrons[level] -> Fill( elec1.Perp()*(1./GeV) );
    h_pt_electrons[level] -> Fill( elec2.Perp()*(1./GeV) );

    h_eta_electrons[level] -> Fill( elec1.Rapidity() );
    h_eta_electrons[level] -> Fill( elec2.Rapidity() );

    h_phi_electrons[level] -> Fill( elec1.Phi() );
    h_phi_electrons[level] -> Fill( elec2.Phi() );

    h_eoverp_electrons[level] ->Fill( cluster1.E()/track1.E() );
    h_eoverp_electrons[level] ->Fill( cluster2.E()/track2.E() );  

    h_eoverpq_electrons[level] ->Fill( cluster1.E()/track1.E()*charge1 );
    h_eoverpq_electrons[level] ->Fill( cluster2.E()/track2.E()*charge2 );  
  }

  void ZeePlots::fillGenPlots( int nel ){
   h_n_el -> Fill( nel );
  }
  
}
