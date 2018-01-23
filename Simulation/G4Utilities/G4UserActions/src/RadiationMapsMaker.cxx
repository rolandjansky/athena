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
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TGraph.h"
#include "TFile.h"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "PathResolver/PathResolver.h"

namespace G4UA{

  
  RadiationMapsMaker::RadiationMapsMaker()
    : m_nev(0.0)
  {
  }
  
  void RadiationMapsMaker::BeginOfRunAction(const G4Run*){

    m_f = new TFile("RadMaps.root","RECREATE");

    const int nBinsr = 120;
    const int nBinsz = 240;

    const double rMinZoom =    0.; // cm
    const double rMinFull =    0.; // cm

    const double rMaxZoom =  480.; // cm
    const double rMaxFull = 1200.; // cm

    const double zMinZoom =    0.; // cm
    const double zMinFull =    0.; // cm

    const double zMaxZoom =  960.; // cm
    const double zMaxFull = 2400.; // cm


    m_rz_tid  = new TH2D("rz_tid" ,"rz_tid" ,nBinsz,zMinZoom,zMaxZoom,nBinsr,rMinZoom,rMaxZoom);
    m_rz_eion = new TH2D("rz_eion","rz_eion",nBinsz,zMinZoom,zMaxZoom,nBinsr,rMinZoom,rMaxZoom);
    m_rz_niel = new TH2D("rz_niel","rz_niel",nBinsz,zMinZoom,zMaxZoom,nBinsr,rMinZoom,rMaxZoom);
    m_rz_h20  = new TH2D("rz_h20" ,"rz_h20" ,nBinsz,zMinZoom,zMaxZoom,nBinsr,rMinZoom,rMaxZoom);

    m_rz_tid  ->SetXTitle("|z| [cm]");
    m_rz_eion ->SetXTitle("|z| [cm]");
    m_rz_niel ->SetXTitle("|z| [cm]");
    m_rz_h20  ->SetXTitle("|z| [cm]");

    m_rz_tid  ->SetYTitle("r [cm]");
    m_rz_eion ->SetYTitle("r [cm]");
    m_rz_niel ->SetYTitle("r [cm]");
    m_rz_h20  ->SetYTitle("r [cm]");

    m_rz_tid  ->SetZTitle("TID [Gy]");
    m_rz_eion ->SetZTitle("E_{ion}/V [MeV/cm^{3}]");
    m_rz_niel ->SetZTitle("NIEL [n_{eq}/cm^{2}]");
    m_rz_h20  ->SetZTitle("SEE [h_{>20 MeV}/cm^{2}]");

    m_full_rz_tid  = new TH2D("full_rz_tid" ,"full_rz_tid" ,nBinsz,zMinFull,zMaxFull,nBinsr,rMinFull,rMaxFull);
    m_full_rz_eion = new TH2D("full_rz_eion","full_rz_eion",nBinsz,zMinFull,zMaxFull,nBinsr,rMinFull,rMaxFull);
    m_full_rz_niel = new TH2D("full_rz_niel","full_rz_niel",nBinsz,zMinFull,zMaxFull,nBinsr,rMinFull,rMaxFull);
    m_full_rz_h20  = new TH2D("full_rz_h20" ,"full_rz_h20" ,nBinsz,zMinFull,zMaxFull,nBinsr,rMinFull,rMaxFull);

    m_full_rz_tid  ->SetXTitle("|z| [cm]");
    m_full_rz_eion ->SetXTitle("|z| [cm]");
    m_full_rz_niel ->SetXTitle("|z| [cm]");
    m_full_rz_h20  ->SetXTitle("|z| [cm]");

    m_full_rz_tid  ->SetYTitle("r [cm]");
    m_full_rz_eion ->SetYTitle("r [cm]");
    m_full_rz_niel ->SetYTitle("r [cm]");
    m_full_rz_h20  ->SetYTitle("r [cm]");

    m_full_rz_tid  ->SetZTitle("TID [Gy]");
    m_full_rz_eion ->SetZTitle("E_{ion}/V [MeV/cm^{3}]");
    m_full_rz_niel ->SetZTitle("NIEL [n_{eq}/cm^{2}]");
    m_full_rz_h20  ->SetZTitle("SEE [h_{>20 MeV}/cm^{2}]");

    const int nBinsr3d   = 30;
    const int nBinsz3d   = 60;
    const int nBinsphi3d = 32;

    const double phiMinZoom = -180.; // degrees
    const double phiMaxZoom =  180.; // degrees

    m_3d_tid  = new TH3D("h3d_tid" ,"h3d_tid" ,nBinsz3d,zMinZoom,zMaxZoom,nBinsr3d,rMinZoom,rMaxZoom,nBinsphi3d,phiMinZoom,phiMaxZoom);
    m_3d_eion = new TH3D("h3d_eion","h3d_eion",nBinsz3d,zMinZoom,zMaxZoom,nBinsr3d,rMinZoom,rMaxZoom,nBinsphi3d,phiMinZoom,phiMaxZoom);
    m_3d_niel = new TH3D("h3d_niel","h3d_niel",nBinsz3d,zMinZoom,zMaxZoom,nBinsr3d,rMinZoom,rMaxZoom,nBinsphi3d,phiMinZoom,phiMaxZoom);
    m_3d_h20  = new TH3D("h3d_h20" ,"h3d_h20" ,nBinsz3d,zMinZoom,zMaxZoom,nBinsr3d,rMinZoom,rMaxZoom,nBinsphi3d,phiMinZoom,phiMaxZoom);

    m_3d_tid  ->SetXTitle("|z| [cm]");
    m_3d_eion ->SetXTitle("|z| [cm]");
    m_3d_niel ->SetXTitle("|z| [cm]");
    m_3d_h20  ->SetXTitle("|z| [cm]");

    m_3d_tid  ->SetYTitle("r [cm]");
    m_3d_eion ->SetYTitle("r [cm]");
    m_3d_niel ->SetYTitle("r [cm]");
    m_3d_h20  ->SetYTitle("r [cm]");

    m_3d_tid  ->SetZTitle("#phi [#circ]");
    m_3d_eion ->SetZTitle("#phi [#circ]");
    m_3d_niel ->SetZTitle("#phi [#circ]");
    m_3d_h20  ->SetZTitle("#phi [#circ]");

    m_3d_tid  ->SetTitle("TID [Gy]");
    m_3d_eion ->SetTitle("E_{ion}/V [MeV/cm^{3}]");
    m_3d_niel ->SetTitle("NIEL [n_{eq}/cm^{2}]");
    m_3d_h20  ->SetTitle("SEE [h_{>20 MeV}/cm^{2}]");

    std::string fpSiA = PathResolver::find_file("protons_Si_Gunnar_Summers.dat"   ,"DATAPATH");//E_kin < 15 MeV
    std::string fpSiB = PathResolver::find_file("protons_Si_Gunnar_Huhtinen.dat"  ,"DATAPATH");//E_kin > 15 MeV
    std::string fnSiA = PathResolver::find_file("neutrons_Si_Gunnar_Griffin.dat"  ,"DATAPATH");//E_kin < 20 MeV
    std::string fnSiB = PathResolver::find_file("neutrons_Si_Gunnar_Konobeyev.dat","DATAPATH");//E_kin > 20 MeV
    std::string fpiSi = PathResolver::find_file("pions_Si_Gunnar_Huhtinen.dat"    ,"DATAPATH");//E_kin > 15 MeV

    m_tgpSiA = new TGraph(fpSiA.c_str()); //E_kin < 15 MeV
    m_tgpSiB = new TGraph(fpSiB.c_str()); //E_kin > 15 MeV
    m_tgnSiA = new TGraph(fnSiA.c_str()); //E_kin < 20 MeV
    m_tgnSiB = new TGraph(fnSiB.c_str()); //E_kin > 20 MeV
    m_tgpiSi = new TGraph(fpiSi.c_str()); //E_kin > 15 MeV
  }
  
  void RadiationMapsMaker::EndOfEventAction(const G4Event*){;
    m_nev+=1.;
  }
  
  void RadiationMapsMaker::EndOfRunAction(const G4Run*){
    m_f->cd();
    // normalize to volume element per bin
    for(int i=0;i<m_rz_tid->GetNbinsX();i++) { 
      for(int j=0;j<m_rz_tid->GetNbinsY();j++) { 
	int iBin = m_rz_tid->GetBin(i+1,j+1); 
	double r0=m_rz_tid->GetYaxis()->GetBinLowEdge(j+1);
	double r1=m_rz_tid->GetYaxis()->GetBinUpEdge(j+1);
	double z0=m_rz_tid->GetXaxis()->GetBinLowEdge(i+1);
	double z1=m_rz_tid->GetXaxis()->GetBinUpEdge(i+1); 
	double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0); 
	double val;
	// TID
	val =m_rz_tid->GetBinContent(iBin); 
	m_rz_tid->SetBinContent(iBin,val/vol);
	// EION
	val =m_rz_eion->GetBinContent(iBin); 
	m_rz_eion->SetBinContent(iBin,val/vol);
	// NIEL
	val =m_rz_niel->GetBinContent(iBin); 
	m_rz_niel->SetBinContent(iBin,val/vol);
	// SEE
	val =m_rz_h20->GetBinContent(iBin); 
	m_rz_h20->SetBinContent(iBin,val/vol);
      }
    }
    m_rz_tid->Write();
    m_rz_eion->Write();
    m_rz_niel->Write();
    m_rz_h20->Write();

    // normalize to volume element per bin
    for(int i=0;i<m_full_rz_tid->GetNbinsX();i++) { 
      for(int j=0;j<m_full_rz_tid->GetNbinsY();j++) { 
	int iBin = m_full_rz_tid->GetBin(i+1,j+1); 
	double r0=m_full_rz_tid->GetYaxis()->GetBinLowEdge(j+1);
	double r1=m_full_rz_tid->GetYaxis()->GetBinUpEdge(j+1);
	double z0=m_full_rz_tid->GetXaxis()->GetBinLowEdge(i+1);
	double z1=m_full_rz_tid->GetXaxis()->GetBinUpEdge(i+1); 
	double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0); 
	double val;
	// TID
	val =m_full_rz_tid->GetBinContent(iBin); 
	m_full_rz_tid->SetBinContent(iBin,val/vol);
	// EION
	val =m_full_rz_eion->GetBinContent(iBin); 
	m_full_rz_eion->SetBinContent(iBin,val/vol);
	// NIEL
	val =m_full_rz_niel->GetBinContent(iBin); 
	m_full_rz_niel->SetBinContent(iBin,val/vol);
	// SEE
	val =m_full_rz_h20->GetBinContent(iBin); 
	m_full_rz_h20->SetBinContent(iBin,val/vol);
      }
    }
    m_full_rz_tid->Write();
    m_full_rz_eion->Write();
    m_full_rz_niel->Write();
    m_full_rz_h20->Write();

    // normalize to volume element per bin
    for(int i=0;i<m_3d_tid->GetNbinsX();i++) { /* |z| */
      for(int j=0;j<m_3d_tid->GetNbinsY();j++) { /* r */
	for(int k=0;k<m_3d_tid->GetNbinsZ();k++) { /* phi */
	  int iBin = m_3d_tid->GetBin(i+1,j+1,k+1); 
	  double phi0=m_3d_tid->GetZaxis()->GetBinLowEdge(k+1);
	  double phi1=m_3d_tid->GetZaxis()->GetBinUpEdge(k+1);
	  double r0=m_3d_tid->GetYaxis()->GetBinLowEdge(j+1);
	  double r1=m_3d_tid->GetYaxis()->GetBinUpEdge(j+1);
	  double z0=m_3d_tid->GetXaxis()->GetBinLowEdge(i+1);
	  double z1=m_3d_tid->GetXaxis()->GetBinUpEdge(i+1); 
	  double vol=2*(z1-z0)*M_PI*(r1*r1-r0*r0)*(phi1-phi0)/360.; 
	  double val;
	  // TID
	  val =m_3d_tid->GetBinContent(iBin); 
	  m_3d_tid->SetBinContent(iBin,val/vol);
	  // EION
	  val =m_3d_eion->GetBinContent(iBin); 
	  m_3d_eion->SetBinContent(iBin,val/vol);
	  // NIEL
	  val =m_3d_niel->GetBinContent(iBin); 
	  m_3d_niel->SetBinContent(iBin,val/vol);
	  // SEE
	  val =m_3d_h20->GetBinContent(iBin); 
	  m_3d_h20->SetBinContent(iBin,val/vol);
	}
      }
    }
    m_3d_tid->Write();
    m_3d_eion->Write();
    m_3d_niel->Write();
    m_3d_h20->Write();

    m_f->Close();
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
	  
	  m_rz_tid       ->Fill(absz,rr,dE_ION/rho);
	  m_full_rz_tid  ->Fill(absz,rr,dE_ION/rho);
	  m_3d_tid       ->Fill(absz,rr,pphi,dE_ION/rho);
	  m_rz_eion      ->Fill(absz,rr,dE_ION);
	  m_full_rz_eion ->Fill(absz,rr,dE_ION);
	  m_3d_eion      ->Fill(absz,rr,pphi,dE_ION);

	  if ( pdgid == 1 || pdgid == 2 || pdgid == 6 || pdgid == 7 ) {
	    
	    if ( weight > 0 ) {
	      m_rz_niel     ->Fill(absz,rr,weight*dl);
	      m_full_rz_niel->Fill(absz,rr,weight*dl);
	      m_3d_niel     ->Fill(absz,rr,pphi,weight*dl);
	    }
	    if ( eKin > 20 ) {
	      m_rz_h20     ->Fill(absz,rr,dl);
	      m_full_rz_h20->Fill(absz,rr,dl);
	      m_3d_h20     ->Fill(absz,rr,pphi,dl);
	    }
	  }
	}
      }
    }
  }
} // namespace G4UA 
  
