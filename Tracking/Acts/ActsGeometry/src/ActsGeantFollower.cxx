/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollower.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ActsGeometry/ActsGeantFollower.h"
#include "ActsGeometry/IActsGeantFollowerHelper.h"
#include "CxxUtils/AthUnlikelyMacros.h"

#include "G4Event.hh"
#include "G4Step.hh"
#include "G4Material.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"
#include "G4DynamicParticle.hh"
#include "G4Track.hh"

ActsGeantFollower::ActsGeantFollower(){}

void ActsGeantFollower::BeginOfEventAction(const G4Event*)
{
  m_helper->beginEvent();
}

void ActsGeantFollower::EndOfEventAction(const G4Event*)
{
  m_helper->endEvent();
}

void ActsGeantFollower::BeginOfRunAction(const G4Run*)
{
  if(m_helper.retrieve()!=StatusCode::SUCCESS)
    {
      G4ExceptionDescription description;
      description << "Cannot retrieve ActsGeantFollower helper";
      G4Exception("ActsGeantFollower", "ActsGeantFollower1", FatalException, description);
      return;
    }
  return;
}

void ActsGeantFollower::UserSteppingAction(const G4Step* aStep)
{
  // kill secondaries and low momentum particles
  if (aStep->GetTrack()->GetParentID() || aStep->GetPreStepPoint()->GetMomentum().mag()<500 )
    {
      aStep->GetTrack()->SetTrackStatus(fStopAndKill);
      return;
    }

  // get the prestep point and follow this guy
  G4StepPoint * g4PreStep  = aStep->GetPreStepPoint();
  G4ThreeVector g4Momentum = g4PreStep->GetMomentum();
  G4ThreeVector g4Position = g4PreStep->GetPosition();

  G4Track* g4Track = aStep->GetTrack();
  const G4DynamicParticle* g4DynParticle = g4Track->GetDynamicParticle();

  // the material information
  const G4TouchableHistory* touchHist = static_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());
  if(ATH_LIKELY(touchHist))
    {
      // G4LogicalVolume
      const G4LogicalVolume *lv= touchHist->GetVolume()->GetLogicalVolume();
      if(ATH_LIKELY(lv))
        {
          const G4Material *mat    = lv->GetMaterial();
           // the step information
          double steplength     = aStep->GetStepLength();
          // the position information
          double X0             = mat->GetRadlen();
          // update the track follower
          m_helper->trackParticle(g4Position,g4Momentum,g4DynParticle->GetPDGcode(),g4DynParticle->GetCharge(),steplength,X0);
        }
      else
         {
          G4ExceptionDescription description;
          description << "ActsGeantFollower::SteppingAction NULL G4LogicalVolume pointer.";
          G4Exception("ActsGeantFollower", "ActsGeantFollower2", FatalException, description);
        }
    }
  else
    {
      G4ExceptionDescription description;
      description << "ActsGeantFollower::SteppingAction NULL G4TouchableHistory pointer.";
      G4Exception("ActsGeantFollower", "ActsGeantFollower3", FatalException, description);
    }
  return;
}
