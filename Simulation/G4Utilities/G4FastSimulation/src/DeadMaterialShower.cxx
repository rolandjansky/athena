/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DeadMaterialShower.h"

#include "G4FastTrack.hh"
#include "G4FastStep.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"

DeadMaterialShower::DeadMaterialShower(const std::string& name, const double& highEnergy, const double& lowEnergy, const double& zcutoff)
  : G4VFastSimulationModel(name),
    m_highEnergy(highEnergy),
    m_lowEnergy(lowEnergy),
    m_zcutoff(zcutoff)
{

}

G4bool DeadMaterialShower::IsApplicable(const G4ParticleDefinition& particleType)
{
  // Apply parameterization only to Electrons, Positrons, and Photons.  ModelTrigger will not be called if this returns false
  if ( &particleType == G4Electron::ElectronDefinition()    ||
       &particleType == G4Positron::PositronDefinition()    ||
       &particleType == G4PionPlus::PionPlusDefinition()    ||
       &particleType == G4PionMinus::PionMinusDefinition()  ||
       &particleType == G4Gamma::GammaDefinition()          ||
       &particleType == G4Neutron::NeutronDefinition()
       )
    {
      return true;
    }
  return false;

}

G4bool DeadMaterialShower::ModelTrigger(const G4FastTrack& fastTrack)
{
  if ( fabs( fastTrack.GetPrimaryTrack()->GetPosition().z() ) < m_zcutoff ){
    // In front of the cut - kill e+/- up to low energy
    if ( fastTrack.GetPrimaryTrack()->GetKineticEnergy() < m_lowEnergy &&
         (fastTrack.GetPrimaryTrack()->GetDefinition() == G4Electron::Definition() ||
          fastTrack.GetPrimaryTrack()->GetDefinition() == G4Positron::Definition() ) )
      {
        return true;
      }
    return false;
  }

  // Behind the cut - kill !muons up to high energy
  if ( fastTrack.GetPrimaryTrack()->GetKineticEnergy() > m_highEnergy ||
       fastTrack.GetPrimaryTrack()->GetDefinition() == G4MuonPlus::Definition() ||
       fastTrack.GetPrimaryTrack()->GetDefinition() == G4MuonMinus::Definition() )
    {
      return false;
    }
  return true;
}

void DeadMaterialShower::DoIt(const G4FastTrack&, G4FastStep& fastStep)
{
  fastStep.KillPrimaryTrack();
  fastStep.SetPrimaryTrackPathLength(0.0);
}
