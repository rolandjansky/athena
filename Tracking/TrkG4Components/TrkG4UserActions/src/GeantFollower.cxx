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


static GeantFollower sntp("GeantFollower");

GeantFollower::GeantFollower(std::string s)
: FADS::UserAction(s),
  m_name(s),
  m_helper("Trk::GeantFollowerHelper/"+s),
  m_helperPointer(0)
{
    
}


void GeantFollower::BeginOfEventAction(const G4Event*)
{
    // retrieve the helper if it's not already available
    if (!m_helperPointer){
        if (m_helper.retrieve().isFailure()){
            std::cout << " Ouch ... this is nasty ... " << std::endl;
            throw std::runtime_error("Failed to retrieve Trk::IGeantFollowerHelper handle.");
        } else {
            m_helperPointer = (&(*m_helper));    
        }
    }
    // now initialize the helper
    m_helperPointer->beginEvent();
}


void GeantFollower::EndOfEventAction(const G4Event*)
{
    // release event
    m_helperPointer->endEvent();
}


void GeantFollower::BeginOfRunAction(const G4Run*)
{}


void GeantFollower::EndOfRunAction(const G4Run*)
{}


void GeantFollower::SteppingAction(const G4Step* aStep)
{
    
    // kill secondaries
    if (aStep->GetTrack()->GetParentID()) {
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
	m_helperPointer->trackParticle(g4Position,g4Momentum,g4DynParticle->GetPDGcode(),g4DynParticle->GetCharge(),steplength,X0);
      }
      else {
	throw std::runtime_error("GeantFollower::SteppingAction NULL G4LogicalVolume pointer.");
      }
    }
    else {
      throw std::runtime_error("GeantFollower::SteppingAction NULL G4TouchableHistory pointer.");
    }
    return;    
}



