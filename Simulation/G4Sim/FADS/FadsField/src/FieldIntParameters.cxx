/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/FieldIntParameters.h"

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
//#include "G4ChargedGeantino.hh"

namespace FADS {

G4double FieldIntParameters::default_kineticEnergyThreshold = 1.0*CLHEP::GeV;
G4double FieldIntParameters::default_maxLengthForConstField = 1.0*CLHEP::mm;
G4double FieldIntParameters::default_missDistance = 0.25*CLHEP::mm;
G4double FieldIntParameters::default_typicalLongStep = 1.0*CLHEP::cm;
G4double FieldIntParameters::default_tolerableBiasError = 1.0e-5*CLHEP::m;
G4double FieldIntParameters::default_tolerableIntegrationError = 1.0e-4*CLHEP::m;
G4double FieldIntParameters::default_expectedNumBoundaries = 1.0e2;
G4double FieldIntParameters::default_expectedTrackLength = 20.0*CLHEP::m;

FieldIntParameters::FieldIntParameters()
{
 current_kineticEnergyThreshold = default_kineticEnergyThreshold;

 // for e+/e- track below current_kineticEnergyThreshold
 current_maxLengthForConstField[0] = default_maxLengthForConstField * 100.;
 current_missDistance[0] = default_missDistance;
 current_typicalLongStep[0] = default_typicalLongStep;
 current_tolerableBiasError[0] = default_tolerableBiasError;
 current_tolerableIntegrationError[0] = default_tolerableIntegrationError * 10.;
 current_expectedNumBoundaries[0] = default_expectedNumBoundaries / 10.;
 current_expectedTrackLength[0] = default_expectedTrackLength / 100.;

 // for e+/e- track above current_kineticEnergyThreshold
 // and all other charged track except mu+/mu- of any kinetic energy
 current_maxLengthForConstField[1] = default_maxLengthForConstField / 1000.;
 current_missDistance[1] = default_missDistance;
 current_typicalLongStep[1] = default_typicalLongStep;
 current_tolerableBiasError[1] = default_tolerableBiasError;
 current_tolerableIntegrationError[1] = default_tolerableIntegrationError;
 current_expectedNumBoundaries[1] = default_expectedNumBoundaries;
 current_expectedTrackLength[1] = default_expectedTrackLength;

 // for mu+/mu- track of any kinetic energy
 current_maxLengthForConstField[2] = default_maxLengthForConstField / 1000.;
 current_missDistance[2] = default_missDistance;
 current_typicalLongStep[2] = default_typicalLongStep;
 current_tolerableBiasError[2] = default_tolerableBiasError / 10.;
 current_tolerableIntegrationError[2] = default_tolerableIntegrationError;
 current_expectedNumBoundaries[2] = default_expectedNumBoundaries;
 current_expectedTrackLength[2] = default_expectedTrackLength;

 for(int i=0;i<3;i++)
 {
   nLongStep[i] = 0;
   nShortStep[i] = 0;
 }
 
 idx = -1;
}

FieldIntParameters::~FieldIntParameters()
{;}

void FieldIntParameters::List(G4int iFlg) const
{
  if(iFlg)
  { 
  G4cout<<" Number of Long Steps for each pType \t"<<nLongStep[0]<<"\t"<<nLongStep[1]
          <<"\t"<<nLongStep[2]<<"\t-- Total long steps "<<nLongStep[0]+nLongStep[1]+nLongStep[2]<<G4endl;
  G4cout<<" Number of Short Steps for each pType\t"<<nShortStep[0]<<"\t"<<nShortStep[1]
          <<"\t"<<nShortStep[2]<<"\t-- Total short steps "<<nShortStep[0]+nShortStep[1]+nShortStep[2]<<G4endl;
  }
  G4cout<<" kineticEnergyThreshold [MeV]        \t"<<current_kineticEnergyThreshold<<G4endl;
  G4cout<<" maxLengthForConstField [mm]         \t"<<current_maxLengthForConstField[0]<<"\t"
        <<current_maxLengthForConstField[1]<<"\t"
        <<current_maxLengthForConstField[2]<<G4endl;
  G4cout<<" missDistance [mm]                   \t"<<current_missDistance[0]<<"\t"
        <<current_missDistance[1]<<"\t"
        <<current_missDistance[2]<<G4endl;
  G4cout<<" tolerableBiasError [mm]             \t"<<current_tolerableBiasError[0]<<"\t"
        <<current_tolerableBiasError[1]<<"\t"
        <<current_tolerableBiasError[2]<<G4endl;
  G4cout<<" tolerableIntegrationError [mm]      \t"<<current_tolerableIntegrationError[0]<<"\t"
        <<current_tolerableIntegrationError[1]<<"\t"
        <<current_tolerableIntegrationError[2]<<G4endl;
}

void FieldIntParameters::GetParameters(const G4Track* aTrack,
    G4double& maxLengthForConstField,
    G4double& missDistance,
    G4double& typicalLongStep,
    G4double& tolerableBiasError,
    G4double& tolerableIntegrationError,
    G4double& expectedNumBoundaries,
    G4double& expectedTrackLength)
{
  idx = 1;
  G4ParticleDefinition* partDef = aTrack->GetDefinition();
  if(partDef==G4Electron::Definition() || partDef==G4Positron::Definition())
  { if(aTrack->GetKineticEnergy() < current_kineticEnergyThreshold) idx = 0; }
  else if(partDef==G4MuonPlus::Definition() || partDef==G4MuonMinus::Definition())
  { idx = 2; }

  maxLengthForConstField = current_maxLengthForConstField[idx];
  missDistance = current_missDistance[idx];
  typicalLongStep = current_typicalLongStep[idx];
  tolerableBiasError = current_tolerableBiasError[idx];
  tolerableIntegrationError = current_tolerableIntegrationError[idx];
  expectedNumBoundaries = current_expectedNumBoundaries[idx];
  expectedTrackLength = current_expectedTrackLength[idx];
}

}

