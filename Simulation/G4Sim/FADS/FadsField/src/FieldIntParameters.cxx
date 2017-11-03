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

G4double FieldIntParameters::s_default_kineticEnergyThreshold = 1.0*CLHEP::GeV;
G4double FieldIntParameters::s_default_maxLengthForConstField = 1.0*CLHEP::mm;
G4double FieldIntParameters::s_default_missDistance = 0.25*CLHEP::mm;
G4double FieldIntParameters::s_default_typicalLongStep = 1.0*CLHEP::cm;
G4double FieldIntParameters::s_default_tolerableBiasError = 1.0e-5*CLHEP::m;
G4double FieldIntParameters::s_default_tolerableIntegrationError = 1.0e-4*CLHEP::m;
G4double FieldIntParameters::s_default_expectedNumBoundaries = 1.0e2;
G4double FieldIntParameters::s_default_expectedTrackLength = 20.0*CLHEP::m;

FieldIntParameters::FieldIntParameters()
{
 m_current_kineticEnergyThreshold = s_default_kineticEnergyThreshold;

 // for e+/e- track below m_current_kineticEnergyThreshold
 m_current_maxLengthForConstField[0] = s_default_maxLengthForConstField * 100.;
 m_current_missDistance[0] = s_default_missDistance;
 m_current_typicalLongStep[0] = s_default_typicalLongStep;
 m_current_tolerableBiasError[0] = s_default_tolerableBiasError;
 m_current_tolerableIntegrationError[0] = s_default_tolerableIntegrationError * 10.;
 m_current_expectedNumBoundaries[0] = s_default_expectedNumBoundaries / 10.;
 m_current_expectedTrackLength[0] = s_default_expectedTrackLength / 100.;

 // for e+/e- track above m_current_kineticEnergyThreshold
 // and all other charged track except mu+/mu- of any kinetic energy
 m_current_maxLengthForConstField[1] = s_default_maxLengthForConstField / 1000.;
 m_current_missDistance[1] = s_default_missDistance;
 m_current_typicalLongStep[1] = s_default_typicalLongStep;
 m_current_tolerableBiasError[1] = s_default_tolerableBiasError;
 m_current_tolerableIntegrationError[1] = s_default_tolerableIntegrationError;
 m_current_expectedNumBoundaries[1] = s_default_expectedNumBoundaries;
 m_current_expectedTrackLength[1] = s_default_expectedTrackLength;

 // for mu+/mu- track of any kinetic energy
 m_current_maxLengthForConstField[2] = s_default_maxLengthForConstField / 1000.;
 m_current_missDistance[2] = s_default_missDistance;
 m_current_typicalLongStep[2] = s_default_typicalLongStep;
 m_current_tolerableBiasError[2] = s_default_tolerableBiasError / 10.;
 m_current_tolerableIntegrationError[2] = s_default_tolerableIntegrationError;
 m_current_expectedNumBoundaries[2] = s_default_expectedNumBoundaries;
 m_current_expectedTrackLength[2] = s_default_expectedTrackLength;

 for(int i=0;i<3;i++)
 {
   m_nLongStep[i] = 0;
   m_nShortStep[i] = 0;
 }
 
 m_idx = -1;
}

FieldIntParameters::~FieldIntParameters()
{;}

void FieldIntParameters::List(G4int iFlg) const
{
  if(iFlg)
  { 
  G4cout<<" Number of Long Steps for each pType \t"<<m_nLongStep[0]<<"\t"<<m_nLongStep[1]
          <<"\t"<<m_nLongStep[2]<<"\t-- Total long steps "<<m_nLongStep[0]+m_nLongStep[1]+m_nLongStep[2]<<G4endl;
  G4cout<<" Number of Short Steps for each pType\t"<<m_nShortStep[0]<<"\t"<<m_nShortStep[1]
          <<"\t"<<m_nShortStep[2]<<"\t-- Total short steps "<<m_nShortStep[0]+m_nShortStep[1]+m_nShortStep[2]<<G4endl;
  }
  G4cout<<" kineticEnergyThreshold [MeV]        \t"<<m_current_kineticEnergyThreshold<<G4endl;
  G4cout<<" maxLengthForConstField [mm]         \t"<<m_current_maxLengthForConstField[0]<<"\t"
        <<m_current_maxLengthForConstField[1]<<"\t"
        <<m_current_maxLengthForConstField[2]<<G4endl;
  G4cout<<" missDistance [mm]                   \t"<<m_current_missDistance[0]<<"\t"
        <<m_current_missDistance[1]<<"\t"
        <<m_current_missDistance[2]<<G4endl;
  G4cout<<" tolerableBiasError [mm]             \t"<<m_current_tolerableBiasError[0]<<"\t"
        <<m_current_tolerableBiasError[1]<<"\t"
        <<m_current_tolerableBiasError[2]<<G4endl;
  G4cout<<" tolerableIntegrationError [mm]      \t"<<m_current_tolerableIntegrationError[0]<<"\t"
        <<m_current_tolerableIntegrationError[1]<<"\t"
        <<m_current_tolerableIntegrationError[2]<<G4endl;
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
  m_idx = 1;
  G4ParticleDefinition* partDef = aTrack->GetDefinition();
  if(partDef==G4Electron::Definition() || partDef==G4Positron::Definition())
  { if(aTrack->GetKineticEnergy() < m_current_kineticEnergyThreshold) m_idx = 0; }
  else if(partDef==G4MuonPlus::Definition() || partDef==G4MuonMinus::Definition())
  { m_idx = 2; }

  maxLengthForConstField = m_current_maxLengthForConstField[m_idx];
  missDistance = m_current_missDistance[m_idx];
  typicalLongStep = m_current_typicalLongStep[m_idx];
  tolerableBiasError = m_current_tolerableBiasError[m_idx];
  tolerableIntegrationError = m_current_tolerableIntegrationError[m_idx];
  expectedNumBoundaries = m_current_expectedNumBoundaries[m_idx];
  expectedTrackLength = m_current_expectedTrackLength[m_idx];
}

}

