/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SteppingValidation.h"
#include <iostream>
#include <cmath>

#include <TH1.h>
#include <TH2.h>

#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4VProcess.hh"
#include "G4Track.hh"

#include "MCTruth/TrackHelper.h"



namespace G4UA{

  
  void SteppingValidation::BeginOfRunAction(const G4Run*){
    m_path += "Stepping/";
    
    // Set up all the histograms...
    _TH1D_NOCHECK( m_stepL , "stepL" , 100 , -9 , 3 );
    _TH1D_NOCHECK( m_stepProc , "stepProc" , 15 , -0.5 , 14.5 );
    _TH1D_NOCHECK( m_mscAngle , "mscAngle" , 100 , 0. , M_PI );
    _TH1D_NOCHECK( m_stepELoss , "stepELoss" , 100 , -10 , 2 );
    _TH1D_NOCHECK( m_secE , "secE" , 100 , -3.5 , 5 );
    _TH2D_NOCHECK( m_EvsR , "EvsR" , 100 , 0 , 1.2 , 100 , 1000. , 20000.);
    _TH1D_NOCHECK( m_latPhi , "latPhi" , 100 , 0. , 1.2 );
    _TH1D_NOCHECK( m_latEta , "latEta" , 100 , 0. , 0.6 );
    
    _SET_TITLE(m_stepL,     "Step length distribution",               "Step length [log(mm)]",  "Steps");
    _SET_TITLE(m_stepProc,  "Step process distribution",              "Step process",           "Steps");
    _SET_TITLE(m_mscAngle,  "Multiple scattering angle distribution", "Angle",                  "Steps");
    _SET_TITLE(m_stepELoss, "Energy loss distribution",               "Energy Loss [log(MeV)]", "Steps");
    _SET_TITLE(m_secE,      "Secondary energy distribution",          "Energy [log(MeV)]",      "Secondaries");
    _SET_TITLE(m_EvsR,      "Primary Energy vs Radius",               "Radius [m]",             "Primary Energy [MeV]");
    _SET_TITLE(m_latPhi,    "Phi energy distribution",                "Energy-weighted #phi RMS",    "Primaries");
    _SET_TITLE(m_latEta,    "Eta energy distribution",                "Energy-weighted #eta RMS",    "Primaries");
    
  }
  
  void SteppingValidation::EndOfEventAction(const G4Event*){
    // Fill lateral energy spread 
    if (m_nsec>0){
      m_latPhi->Fill( std::sqrt( m_dp2/m_nsec - std::pow(m_dp/m_nsec,2) ) );
      m_latEta->Fill( std::sqrt( m_dh2/m_nsec - std::pow(m_dh/m_nsec,2) ) );
    }
  }
  
  void SteppingValidation::BeginOfEventAction(const G4Event*){
    m_prim=m_sec=0;
    m_primH=m_primF=0;
    m_dh=m_dh2=m_dp=m_dp2=0;
    m_nsec=0;
  }
  
  void SteppingValidation::UserSteppingAction(const G4Step* aStep){
    // Fill process type
    m_stepProc->Fill(aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType());
    
    // Fill energy deposit
    m_stepELoss->Fill( log10(aStep->GetTotalEnergyDeposit()) );
    
    // Fill step length
    m_stepL->Fill( log10( aStep->GetStepLength() ) );
    
    // Fill secondary energies
    if (m_prim==0){
      m_prim = aStep->GetTrack();
    } else if (aStep->GetTrack()!=m_prim && aStep->GetTrack()!=m_sec){ // New secondary
      m_secE->Fill( log10(aStep->GetPreStepPoint()->GetTotalEnergy()) );
      m_sec=aStep->GetTrack();
    }
    
    if (aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType()==1){ // Defined by transport --> MSC
      if (m_prim==aStep->GetTrack()){
	double angle = std::acos( aStep->GetPreStepPoint()->GetMomentumDirection().dot( aStep->GetPostStepPoint()->GetMomentumDirection() ) );
	m_mscAngle->Fill(angle);
	m_EvsR->Fill( aStep->GetPostStepPoint()->GetPosition().perp()*0.001 , aStep->GetTrack()->GetTotalEnergy() );
      }
    }
    
    if (aStep->GetTrack()->GetNextVolume()==0){
      // Leaving world!  Use this for RMS calculations
      if (m_prim==aStep->GetTrack()){
	m_primH = aStep->GetPostStepPoint()->GetPosition().eta();
	m_primF = aStep->GetPostStepPoint()->GetPosition().phi();
      } else {
	m_dh  +=         (aStep->GetPostStepPoint()->GetPosition().eta() - m_primH  )*aStep->GetTrack()->GetTotalEnergy();
	m_dh2 += std::pow(aStep->GetPostStepPoint()->GetPosition().eta() - m_primH,2)*aStep->GetTrack()->GetTotalEnergy();
	m_dp  +=         (aStep->GetPostStepPoint()->GetPosition().phi() - m_primF  )*aStep->GetTrack()->GetTotalEnergy();
	m_dp2 += std::pow(aStep->GetPostStepPoint()->GetPosition().phi() - m_primF,2)*aStep->GetTrack()->GetTotalEnergy();
	m_nsec += aStep->GetTrack()->GetTotalEnergy();
    }
    }
    
  }
  
} // namespace G4UA 
