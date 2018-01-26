/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/RadiationMapsMaker.h"
#include <iostream>
#include <string>
#include <cmath>
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"
#include "G4Gamma.hh"
#include "TGraph.h"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "PathResolver/PathResolver.h"

namespace G4UA{

  
  //---------------------------------------------------------------------------
  // Merge results
  //---------------------------------------------------------------------------
  void RadiationMapsMaker::Report::merge(const RadiationMapsMaker::Report& maps)
  {

    // all 2d vectors have same size 
    for(unsigned int i=0;i<maps.m_rz_tid.size();i++) {
      // zoom 2d
      m_rz_tid [i] += maps.m_rz_tid [i];
      m_rz_eion[i] += maps.m_rz_eion[i];
      m_rz_niel[i] += maps.m_rz_niel[i];
      m_rz_h20 [i] += maps.m_rz_h20 [i];

      // full 2d
      m_full_rz_tid [i] += maps.m_full_rz_tid [i];
      m_full_rz_eion[i] += maps.m_full_rz_eion[i];
      m_full_rz_niel[i] += maps.m_full_rz_niel[i];
      m_full_rz_h20 [i] += maps.m_full_rz_h20 [i];
    }

    // all zoom 3d vectors have same size 
    for(unsigned int i=0;i<maps.m_3d_tid.size();i++) {
      // zoom 3d
      m_3d_tid [i] += maps.m_3d_tid [i];
      m_3d_eion[i] += maps.m_3d_eion[i];
      m_3d_niel[i] += maps.m_3d_niel[i];
      m_3d_h20 [i] += maps.m_3d_h20 [i];
    }
  }

  void RadiationMapsMaker::BeginOfRunAction(const G4Run*){

    // first make sure the vectors are empty

    m_maps.m_rz_tid .resize(0);
    m_maps.m_rz_eion.resize(0);
    m_maps.m_rz_niel.resize(0);
    m_maps.m_rz_h20 .resize(0);
    
    m_maps.m_full_rz_tid .resize(0);
    m_maps.m_full_rz_eion.resize(0);
    m_maps.m_full_rz_niel.resize(0);
    m_maps.m_full_rz_h20 .resize(0);
    
    m_maps.m_3d_tid .resize(0);
    m_maps.m_3d_eion.resize(0);
    m_maps.m_3d_niel.resize(0);
    m_maps.m_3d_h20 .resize(0);

    // then resize to proper size and initialize with 0's 

    m_maps.m_rz_tid .resize(m_maps.nBinsz*m_maps.nBinsr,0.0);
    m_maps.m_rz_eion.resize(m_maps.nBinsz*m_maps.nBinsr,0.0);
    m_maps.m_rz_niel.resize(m_maps.nBinsz*m_maps.nBinsr,0.0);
    m_maps.m_rz_h20 .resize(m_maps.nBinsz*m_maps.nBinsr,0.0);
    
    m_maps.m_full_rz_tid .resize(m_maps.nBinsz*m_maps.nBinsr,0.0);
    m_maps.m_full_rz_eion.resize(m_maps.nBinsz*m_maps.nBinsr,0.0);
    m_maps.m_full_rz_niel.resize(m_maps.nBinsz*m_maps.nBinsr,0.0);
    m_maps.m_full_rz_h20 .resize(m_maps.nBinsz*m_maps.nBinsr,0.0);
    
    m_maps.m_3d_tid .resize(m_maps.nBinsz3d*m_maps.nBinsr3d*m_maps.nBinsphi3d,0.0);
    m_maps.m_3d_eion.resize(m_maps.nBinsz3d*m_maps.nBinsr3d*m_maps.nBinsphi3d,0.0);
    m_maps.m_3d_niel.resize(m_maps.nBinsz3d*m_maps.nBinsr3d*m_maps.nBinsphi3d,0.0);
    m_maps.m_3d_h20 .resize(m_maps.nBinsz3d*m_maps.nBinsr3d*m_maps.nBinsphi3d,0.0);

    /// data files for NIEL damage factors in Si
    ///
    /// The data resides in the share directory of the package and should not be
    /// changed. To prevent modification of the damage factors by accident
    /// the file names are not configurable
    ///
    std::string fpSiA = PathResolver::find_file("protons_Si_Gunnar_Summers.dat"   ,"DATAPATH");//E_kin < 15 MeV
    std::string fpSiB = PathResolver::find_file("protons_Si_Gunnar_Huhtinen.dat"  ,"DATAPATH");//E_kin > 15 MeV
    std::string fnSiA = PathResolver::find_file("neutrons_Si_Gunnar_Griffin.dat"  ,"DATAPATH");//E_kin < 20 MeV
    std::string fnSiB = PathResolver::find_file("neutrons_Si_Gunnar_Konobeyev.dat","DATAPATH");//E_kin > 20 MeV
    std::string fpiSi = PathResolver::find_file("pions_Si_Gunnar_Huhtinen.dat"    ,"DATAPATH");//E_kin > 15 MeV

    if ( !m_tgpSiA ) 
      m_tgpSiA = new TGraph(fpSiA.c_str()); //E_kin < 15 MeV
    if ( !m_tgpSiB ) 
      m_tgpSiB = new TGraph(fpSiB.c_str()); //E_kin > 15 MeV
    if ( !m_tgnSiA ) 
      m_tgnSiA = new TGraph(fnSiA.c_str()); //E_kin < 20 MeV
    if ( !m_tgnSiB ) 
      m_tgnSiB = new TGraph(fnSiB.c_str()); //E_kin > 20 MeV
    if ( !m_tgpiSi ) 
      m_tgpiSi = new TGraph(fpiSi.c_str()); //E_kin > 15 MeV
  }
  
  void RadiationMapsMaker::UserSteppingAction(const G4Step* aStep){
    int pdgid = 8;
    if (aStep->GetTrack()->GetDefinition()==G4Gamma::Definition()){
      pdgid=0;
    } else if (aStep->GetTrack()->GetDefinition()==G4Proton::Definition()){
      pdgid=1;
    } else if (aStep->GetTrack()->GetDefinition()==G4PionPlus::Definition() ||
	       aStep->GetTrack()->GetDefinition()==G4PionMinus::Definition()){
      pdgid=2;
    } else if(aStep->GetTrack()->GetDefinition()==G4MuonPlus::Definition() ||
	      aStep->GetTrack()->GetDefinition()==G4MuonMinus::Definition()){
      pdgid=3;
    } else if(aStep->GetTrack()->GetDefinition()==G4Electron::Definition() ||
	      aStep->GetTrack()->GetDefinition()==G4Positron::Definition()){
      if (aStep->GetTrack()->GetKineticEnergy()>0.5){
	pdgid=4; 
      } else {
	pdgid=5;
      }
    } else if(aStep->GetTrack()->GetDefinition()==G4Neutron::Definition()){
      if (aStep->GetTrack()->GetKineticEnergy()>10.){
	pdgid=6;
      } else {
	pdgid=7;
      }
    } else if (!aStep->GetTrack()->GetDefinition()->GetPDGCharge()) return; // Not one of those and not a primary?
    

    // process NIEL, h20 and Edep particles only

    if ( pdgid == 1 || pdgid == 2 || pdgid == 6 || pdgid == 7 || /* NIEL & h20*/
	 aStep->GetTotalEnergyDeposit() > 0 ) {
      
    
      double rho = aStep->GetTrack()->GetMaterial()->GetDensity()/CLHEP::g*CLHEP::cm3; 

      // fluence dN/da = dl/dV 
      double x0 = aStep->GetPreStepPoint()->GetPosition().x()*0.1;
      double x1 = aStep->GetPostStepPoint()->GetPosition().x()*0.1;
      double y0 = aStep->GetPreStepPoint()->GetPosition().y()*0.1;
      double y1 = aStep->GetPostStepPoint()->GetPosition().y()*0.1;
      double z0 = aStep->GetPreStepPoint()->GetPosition().z()*0.1;
      double z1 = aStep->GetPostStepPoint()->GetPosition().z()*0.1;
      
      double l = sqrt(pow(x1-x0,2)+pow(y1-y0,2)+pow(z1-z0,2));
      // make 1 mm steps but at least 1 step
      double dl0 = 0.1;
      unsigned int nStep = l/dl0+1;
      double dx = (x1-x0)/nStep;
      double dy = (y1-y0)/nStep;
      double dz = (z1-z0)/nStep;
      double dl = l/nStep;
      
      double weight = 0; // weight for NIEL
      double eKin = aStep->GetTrack()->GetKineticEnergy();
      if ( pdgid == 1 ) {
	if ( eKin < 15 ) {
	  if ( eKin > 10 ) {
	    weight = m_tgpSiA->Eval(eKin);
	  }
	}
	else {
	  weight = m_tgpSiB->Eval(eKin);
	}
      }
      else if ( pdgid == 2 ) {
	if ( eKin > 15 ) {
	  weight = m_tgpiSi->Eval(eKin);
	}
      }
      else if ( pdgid == 6 || pdgid == 7 ) {
	if ( eKin < 20 ) {
	  weight = m_tgnSiA->Eval(eKin);
	}
	else {
	  weight = m_tgnSiB->Eval(eKin);
	}
      }
      
      double dE_TOT = aStep->GetTotalEnergyDeposit()/nStep;
      double dE_NIEL = aStep->GetNonIonizingEnergyDeposit()/nStep;
      double dE_ION = dE_TOT-dE_NIEL;
      
      if ( weight > 0 || eKin > 20 || dE_TOT > 0 ) {
	for(unsigned int i=0;i<nStep;i++) {
	  double absz = fabs(z0+dz*(i+0.5));
	  double rr = sqrt(pow(x0+dx*(i+0.5),2)+
			   pow(y0+dy*(i+0.5),2));
	  double pphi = atan2(y0+dy*(i+0.5),x0+dx*(i+0.5))*180/M_PI;

	  int vBinZoom = -1;
	  int vBinFull = -1;
	  int vBin3d   = -1;

	  // zoom 2d
	  if ( m_maps.zMinZoom < absz && 
	       m_maps.zMaxZoom > absz ) {
	    int iz = (absz-m_maps.zMinZoom)/(m_maps.zMaxZoom-m_maps.zMinZoom)*m_maps.nBinsz;
	    if ( m_maps.rMinZoom < rr && 
		 m_maps.rMaxZoom > rr ) {
	      int ir = (rr-m_maps.rMinZoom)/(m_maps.rMaxZoom-m_maps.rMinZoom)*m_maps.nBinsr;
	      vBinZoom = m_maps.nBinsr*iz+ir;
	    }
	  }

	  // full 2d
	  if ( m_maps.zMinFull < absz && 
	       m_maps.zMaxFull > absz ) {
	    int iz = (absz-m_maps.zMinFull)/(m_maps.zMaxFull-m_maps.zMinFull)*m_maps.nBinsz;
	    if ( m_maps.rMinFull < rr && 
		 m_maps.rMaxFull > rr ) {
	      int ir = (rr-m_maps.rMinFull)/(m_maps.rMaxFull-m_maps.rMinFull)*m_maps.nBinsr;
	      vBinFull = m_maps.nBinsr*iz+ir;
	    }
	  }

	  // zoom 3d
	  if ( m_maps.zMinZoom < absz && 
	       m_maps.zMaxZoom > absz ) {
	    int iz = (absz-m_maps.zMinZoom)/(m_maps.zMaxZoom-m_maps.zMinZoom)*m_maps.nBinsz3d;
	    if ( m_maps.rMinZoom < rr && 
		 m_maps.rMaxZoom > rr ) {
	      int ir = (rr-m_maps.rMinZoom)/(m_maps.rMaxZoom-m_maps.rMinZoom)*m_maps.nBinsr3d;
	      if ( m_maps.phiMinZoom < pphi && 
		   m_maps.phiMaxZoom > pphi ) {
	      int iphi = (pphi-m_maps.phiMinZoom)/(m_maps.phiMaxZoom-m_maps.phiMinZoom)*m_maps.nBinsphi3d;
	      vBin3d = m_maps.nBinsr3d*m_maps.nBinsphi3d*iz+m_maps.nBinsphi3d*ir+iphi;
	      }
	    }
	  }
	  
	  if ( vBinZoom >=0 ) {
	    m_maps.m_rz_tid [vBinZoom] += dE_ION/rho;
	    m_maps.m_rz_eion[vBinZoom] += dE_ION;
	  }
	  if ( vBinFull >=0 ) {
	    m_maps.m_full_rz_tid [vBinFull] += dE_ION/rho;
	    m_maps.m_full_rz_eion[vBinFull] += dE_ION;
	  }
	  if ( vBin3d >=0 ) {
	    m_maps.m_3d_tid [vBin3d] += dE_ION/rho;
	    m_maps.m_3d_eion[vBin3d] += dE_ION;
	  }
	    
	  if ( pdgid == 1 || pdgid == 2 || pdgid == 6 || pdgid == 7 ) {
	    
	    if ( weight > 0 ) {
	      if ( vBinZoom >=0 ) {
		m_maps.m_rz_niel [vBinZoom] += weight*dl;
	      }
	      if ( vBinFull >=0 ) {
		m_maps.m_full_rz_niel [vBinFull] += weight*dl;
	      }
	      if ( vBin3d >=0 ) {
		m_maps.m_3d_niel [vBin3d] += weight*dl;
	      }
	    }
	    if ( eKin > 20 ) {
	      if ( vBinZoom >=0 ) {
		m_maps.m_rz_h20 [vBinZoom] += dl;
	      }
	      if ( vBinFull >=0 ) {
		m_maps.m_full_rz_h20 [vBinFull] += dl;
	      }
	      if ( vBin3d >=0 ) {
		m_maps.m_3d_h20 [vBin3d] += dl;
	      }
	    }
	  }
	}
      }
    }
  }
} // namespace G4UA 
  
