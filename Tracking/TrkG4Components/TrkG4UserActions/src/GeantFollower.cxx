/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollower.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkG4UserActions/GeantFollower.h"
#include "TrkG4UserActions/IGeantFollowerHelper.h"
#include "CxxUtils/AthUnlikelyMacros.h"
#include <iostream>
#include <stdexcept>
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VTrajectoryPoint.hh"
#include "G4DynamicParticle.hh"
#include "G4Track.hh"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

namespace G4UA{


  GeantFollower::GeantFollower(const Config& config)
    : m_config(config)
    , m_helperPointer(nullptr)
  {}

  void GeantFollower::BeginOfEventAction(const G4Event*)
  {
    m_helperPointer->beginEvent();
  }

  void GeantFollower::EndOfEventAction(const G4Event*)
  {
    m_helperPointer->endEvent();
  }

  void GeantFollower::BeginOfRunAction(const G4Run*)
  {
    if(m_config.helper.retrieve()!=StatusCode::SUCCESS)
      {
        G4ExceptionDescription description;
        description << "Cannot retrieve GeantFollower helper";
        G4Exception("GeantFollower", "GeantFollower1", FatalException, description);
        return;
      }

    m_helperPointer = (&(*m_config.helper));

    return;
  }

  void GeantFollower::UserSteppingAction(const G4Step* aStep)
  {
    // kill secondaries
    if (aStep->GetTrack()->GetParentID())
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
            m_helperPointer->trackParticle(g4Position,g4Momentum,g4DynParticle->GetPDGcode(),g4DynParticle->GetCharge(),steplength,X0);
          }
        else
          {
            G4ExceptionDescription description;
            description << "GeantFollower::SteppingAction NULL G4LogicalVolume pointer.";
            G4Exception("GeantFollower", "GeantFollower2", FatalException, description);
          }
      }
    else
      {
        G4ExceptionDescription description;
        description << "GeantFollower::SteppingAction NULL G4TouchableHistory pointer.";
        G4Exception("GeantFollower", "GeantFollower3", FatalException, description);
      }
    return;


  }

} // namespace G4UA
