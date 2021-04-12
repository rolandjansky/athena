/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerMS.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkG4UserActions/GeantFollowerMS.h"
#include "TrkG4UserActions/IGeantFollowerMSHelper.h"
#include "CxxUtils/AthUnlikelyMacros.h"

#include "G4Event.hh"
#include "G4Step.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"
#include "G4DynamicParticle.hh"
#include "G4Track.hh"

namespace G4UA
{

  GeantFollowerMS::GeantFollowerMS(const Config& config)
    : m_config(config)
    , m_helperPointer(nullptr)
  {}

  void GeantFollowerMS::BeginOfEventAction(const G4Event*)
  {
    // FIXME: thread-unsafe tool usage in thread-local action? ATLASSIM-3562.
    m_helperPointer->beginEvent();
  }

  void GeantFollowerMS::EndOfEventAction(const G4Event*)
  {
    // FIXME: thread-unsafe tool usage in thread-local action? ATLASSIM-3562.
    m_helperPointer->endEvent();
  }

  void GeantFollowerMS::BeginOfRunAction(const G4Run*)
  {
    if(m_config.helper.retrieve()!=StatusCode::SUCCESS) {
      G4ExceptionDescription description;
      description << "Cannot retrieve GeantFollowerMS helper";
      G4Exception("GeantFollowerMS", "GeantFollowerMS1", FatalException, description);
      return;
    }
    m_helperPointer = (&(*m_config.helper));

    if(m_config.trackingGeometrySvc.retrieve()!=StatusCode::SUCCESS) {
      G4ExceptionDescription description;
      description << "Cannot retrieve TrackingGeometrySvc in GeantFollowerMS";
      G4Exception("GeantFollowerMS", "GeantFollowerMS2", FatalException, description);
      return;
    }

    return;
  }

  void GeantFollowerMS::UserSteppingAction(const G4Step* aStep)
  {
    // kill secondaries
    if (aStep->GetTrack()->GetParentID()) {
      aStep->GetTrack()->SetTrackStatus(fStopAndKill);
      return;
    }

    // get the prestep point and follow this guy
    G4StepPoint * g4PreStep  = aStep->GetPreStepPoint();
    G4ThreeVector g4Momentum = g4PreStep->GetMomentum();
    const G4ThreeVector& g4Position = g4PreStep->GetPosition();

    G4Track* g4Track = aStep->GetTrack();
    const G4DynamicParticle* g4DynParticle = g4Track->GetDynamicParticle();

    // the material information
    const G4TouchableHistory* touchHist =
      static_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());

    if(ATH_LIKELY(touchHist)) {
      // G4LogicalVolume
      const G4LogicalVolume *lv= touchHist->GetVolume()->GetLogicalVolume();

      if(ATH_LIKELY(lv)) {
        const G4Material *mat    = lv->GetMaterial();
        // the step information
        double steplength     = aStep->GetStepLength();
        // the position information
        double X0             = mat->GetRadlen();
        // update the track follower
        m_helperPointer->trackParticle(g4Position, g4Momentum,
                                       g4DynParticle->GetPDGcode(),
                                       g4DynParticle->GetCharge(),
                                       steplength, X0);
      }
      else {
        G4ExceptionDescription description;
        description << "GeantFollowerMS::SteppingAction NULL G4LogicalVolume pointer.";
        G4Exception("GeantFollowerMS", "GeantFollowerMS3", FatalException, description);
      }
    }
    else {
      G4ExceptionDescription description;
      description << "GeantFollowerMS::SteppingAction NULL G4TouchableHistory pointer.";
      G4Exception("GeantFollowerMS", "GeantFollowerMS4", FatalException, description);
    }
    return;
  }

} // namespace G4UA
