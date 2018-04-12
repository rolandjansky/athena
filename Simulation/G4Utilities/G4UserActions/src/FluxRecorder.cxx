/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/FluxRecorder.h"
#include <iostream>
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
#include "TFile.h"
#include "G4Step.hh"
#include "G4StepPoint.hh"


namespace G4UA
{

  FluxRecorder::FluxRecorder()
    : m_nev(0.0)
  {
  }

  void FluxRecorder::BeginOfRunAction(const G4Run*)
  {
    char nom[120];
    double timebins[101],ebins[101];
    for (int i=0;i<101;++i){
      timebins[i] = std::pow(10.,9.*double(i)/100.);
      ebins[i] = std::pow(10.,-11.+16.*double(i)/100.);
    }
    for (int i=0;i<lastVol;++i){
      for (int j=0;j<9;++j){
        for (int k=0;k<2;++k){
          sprintf(nom,"Flux_%i_%i_%i",i,j,k);
          m_flux[i][j][k] = new TH1D(nom,"",100,timebins);
        } // energy
        sprintf(nom,"FluxE_%i_%i",i,j);
        m_fluxE[i][j] = new TH1D(nom,"",100,ebins);
      } // PDGID
    } // Volume

  }

  void FluxRecorder::EndOfEventAction(const G4Event*)
  {
    m_nev+=1.;
  }

  void FluxRecorder::EndOfRunAction(const G4Run*){
    TFile * f = new TFile("flux.root","RECREATE");
    f->cd();
    char nom[80];
    for (int i=0;i<lastVol;++i){
      for (int j=0;j<9;++j){
        for (int k=0;k<2;++k){
          sprintf(nom,"Flux_%i_%i_%i",i,j,k);
          m_flux[i][j][k]->Scale(1./m_nev);
          m_flux[i][j][k]->Write(nom);
        } // Energy
        sprintf(nom,"FluxE_%i_%i",i,j);
        m_fluxE[i][j]->Scale(1./m_nev);
        m_fluxE[i][j]->Write(nom);
      } // PDGID
    } // Volume
    f->Close();
  }

  void FluxRecorder::UserSteppingAction(const G4Step* aStep){
    int pdgid = 8, energy=(aStep->GetTrack()->GetKineticEnergy()>10.)?1:0;
    if (aStep->GetTrack()->GetDefinition()==G4Gamma::Definition()){
      pdgid=0;
      energy = (aStep->GetTrack()->GetKineticEnergy()>0.03)?1:0;
    } else if (aStep->GetTrack()->GetDefinition()==G4Proton::Definition()){
      pdgid=1;
      energy = (aStep->GetTrack()->GetKineticEnergy()>10.)?1:0;
    } else if (aStep->GetTrack()->GetDefinition()==G4PionPlus::Definition() ||
               aStep->GetTrack()->GetDefinition()==G4PionMinus::Definition()){
      pdgid=2;
      energy = (aStep->GetTrack()->GetKineticEnergy()>10.)?1:0;
    } else if(aStep->GetTrack()->GetDefinition()==G4MuonPlus::Definition() ||
              aStep->GetTrack()->GetDefinition()==G4MuonMinus::Definition()){
      pdgid=3;
      energy = (aStep->GetTrack()->GetKineticEnergy()>10.)?1:0;
    } else if(aStep->GetTrack()->GetDefinition()==G4Electron::Definition() ||
              aStep->GetTrack()->GetDefinition()==G4Positron::Definition()){
      if (aStep->GetTrack()->GetKineticEnergy()>0.5){
        pdgid=4; energy=1;
      } else {
        pdgid=5;
        energy=(aStep->GetTrack()->GetKineticEnergy()>0.01)?1:0;
      }
    } else if(aStep->GetTrack()->GetDefinition()==G4Neutron::Definition()){
      if (aStep->GetTrack()->GetKineticEnergy()>10.){
        pdgid=6; energy=1;
      } else {
        pdgid=7;
        energy=(aStep->GetTrack()->GetKineticEnergy()>0.1?1:0);
      }
    } else if (!aStep->GetTrack()->GetDefinition()->GetPDGCharge()) return; // Not one of those and not a primary?

    m_list.clear();
    findVolume( aStep->GetPreStepPoint()->GetPosition().rho()*0.1, std::fabs(aStep->GetPreStepPoint()->GetPosition().z()*0.1),
                aStep->GetPostStepPoint()->GetPosition().rho()*0.1, std::fabs(aStep->GetPostStepPoint()->GetPosition().z()*0.1) ); // units are cm
    if (m_list.size()==0) return;

    for (unsigned int i=0;i<m_list.size();++i){
      m_flux[m_list[i]][pdgid][energy]->Fill( aStep->GetTrack()->GetGlobalTime() );
      m_fluxE[m_list[i]][pdgid]->Fill( aStep->GetTrack()->GetKineticEnergy() );
    }
  }

  void FluxRecorder::findVolume( const double r1 , const double z1 , const double r2 , const double z2 )
  {

    // This horrible code should be fixed
    const static double dim[lastVol][4] = {
      {980.,1000.,0.,400.} , {980.,1000.,400.,800.} , {980.,1000.,800.,1200.} ,
      {750.,770.,0.,200.} , {750.,770.,200.,450.} , {750.,770.,450.,850.} ,
      {480.,540.,0.,200.} , {480.,540.,200.,400.} , {480.,540.,400.,600.} ,
      {500.,1000.,1320.,1400.} , {280.,500.,1320.,1400.} , {180.,280.,1320.,1400.} ,
      {600.,1200.,2120.,2180.} , {400.,600.,2120.,2180.} , {300.,400.,2120.,2180.} ,
      {450.,600.,700.,760.} , {320.,450.,700.,760.} , {220.,320.,700.,760.} , {100.,200.,720.,760.} , {220.,320.,680.,700.} ,
      {460.,460.1,0.,350.} , {460.,460.1,350.,655.} , {750.,750.1,0.,740.} , {750.,750.1,740.,1000.} ,
      {1020.,1020.1,0.,740.} , {1020.,1020.1,740.,1281.} ,
      {370.,532.,705.,705.1} , {194.,370.,705.,705.1} , {99.,190.,730.,690.} ,
      {683.,1020.,1301.,1301.1} , {264.,683.,1301.,1301.1} , {176.,264.,1301.,1301.1} ,
      {600.,1170.,2040.,2040.1} , {447.,700.,2207.,2207.1} , {298.,447.,2207.,2207.1} ,
      // Extras from Charlie
      {750.,770.,850.,950.} , {480.,540.,600.,720.} };


    for (int i=0;i<lastVol;++i){
      // Crossing outward in r over r1
      int hit = 0;
      if (i!=28){
        double myZ1 = z1+(r1!=r2?(z2-z1)/(r2-r1)*(dim[i][0]-r1):(z2-z1)*0.5);
        double myZ2 = z1+(r1!=r2?(z2-z1)/(r2-r1)*(dim[i][1]-r1):(z2-z1)*0.5);
        double myR1 = r1+(z1!=z2?(r2-r1)/(z2-z1)*(dim[i][2]-z1):(r2-r1)*0.5);
        double myR2 = r1+(z1!=z2?(r2-r1)/(z2-z1)*(dim[i][3]-z1):(r2-r1)*0.5);

        // Crossing outward in r over r1
        if      ( r1<dim[i][0] && r2>dim[i][0] && myZ1>dim[i][2] && myZ1<dim[i][3] ) hit = 1;
        // Crossing inward in r over r2
        else if ( r1>dim[i][1] && r2<dim[i][1] && myZ2>dim[i][2] && myZ2<dim[i][3] ) hit = 2;
        // Crossing rightward in z over z1
        else if ( z1<dim[i][2] && z2>dim[i][2] && myR1>dim[i][0] && myR1<dim[i][1] ) hit = 3;
        // Crossing leftward in z over z2
        else if ( z1>dim[i][3] && z2<dim[i][3] && myR2>dim[i][0] && myR2<dim[i][1] ) hit = 4;

        // Special handling for stupid FLUKA slanted volume
      } else {
        // corner test...
        if ( (r1==dim[i][0]&&z1==dim[i][2])||(r2==dim[i][0]&&r2==dim[i][2]) ) hit = 1;
        else {

          double denom = (r2 - r1)*(dim[i][3] - dim[i][2]) - (z2 - z1)*(dim[i][1] - dim[i][0]);
          double nume_a = (z2 - z1)*(dim[i][0] - r1) - (r2 - r1)*(dim[i][2] - z1);
          double nume_b = (dim[i][3] - dim[i][2])*(dim[i][0] - r1) - (dim[i][1] - dim[i][0])*(dim[i][2] - z1);

          if(!denom){
            if(!nume_a && !nume_b) hit=1; // they are the same line
          } else {
            double ua = nume_a / denom;
            double ub = nume_b / denom;
            if(ua >= 0. && ua <= 1. && ub >= 0. && ub <= 1.) hit=2; // They intersect
          }
        }
      }

      if (hit) {
        m_list.push_back(i);
      } // if we scored
    } // Loop over all volumes
  }

} // namespace G4UA
