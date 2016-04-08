/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HyperspaceCatcher.h"

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PropagatorInField.hh"
#include "G4RunManagerKernel.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TransportationManager.hh"
#include "G4VProcess.hh"
#include "G4VSolid.hh"

#include <cmath>

HyperspaceCatcher::HyperspaceCatcher(const std::string& type, const std::string& name, const IInterface* parent):
  UserActionBase(type,name,parent),
  m_killCount(0),m_world(0),m_treatmentLevel(0),m_killAfter(0)
{
  declareProperty("TreatmentLevel",m_treatmentLevel);
  declareProperty("KillAfter",m_killAfter);
}

void  HyperspaceCatcher::BeginOfRun(const G4Run*)
{
  // Highest level implemented is 2 at the moment
  if (m_treatmentLevel>2) m_treatmentLevel=2;
  ATH_MSG_INFO("Treatment level set to " << m_treatmentLevel );

  ATH_MSG_INFO("Set to kill after set to " << m_killAfter );

  G4LogicalVolumeStore * lvs = G4LogicalVolumeStore::GetInstance();
  for (auto lv : *lvs){
    if ( lv->GetName()=="Atlas::Atlas" ){
      m_world=lv->GetSolid();
      break;
    }
  }
  if (0==m_world){
    ATH_MSG_FATAL( "Could not find world volume!" );
    throw "NoWorld";
  }
}

void HyperspaceCatcher::Step(const G4Step* aStep)
{

  bool hs = false;

  // Various levels of testing.  Simple testing at level 0 : just see if it's in the neighborhood
  G4ThreeVector myPos = aStep->GetPostStepPoint()->GetPosition();
  if (std::fabs( myPos.x() )>30000. ||
      std::fabs( myPos.y() )>30000. ||
      std::fabs( myPos.z() )>50000. ){
    hs=true;
  } // Check #1 failed
  if (!hs && m_treatmentLevel<=0) return;

  if ( m_world->Inside( myPos ) == kOutside ){
    hs=true;
  } // Check #2 failed
  if (!hs && m_treatmentLevel<=1) return;

  G4VSolid * pre = aStep->GetPreStepPoint()->GetPhysicalVolume() &&
    aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume() ?
    aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetSolid():0;
  G4VSolid * post = aStep->GetPostStepPoint()->GetPhysicalVolume() &&
    aStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume() ?
    aStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetSolid():0;
  if ( (post && post->Inside( myPos ) == kOutside ) ||
       ( pre &&  pre->Inside( aStep->GetPreStepPoint()->GetPosition() ) == kOutside ) ){
    hs=true;
  } // Check #3 failed
  if (!hs && m_treatmentLevel<=2) return;

  G4Track *tr = aStep->GetTrack();
  G4ThreeVector mom = tr->GetMomentumDirection();

  std::cout << "Caught a " << tr->GetDefinition()->GetParticleName() << " at (" << myPos.x()
            << ", " << myPos.y() << ", " << myPos.z() << ") to (" << mom.x() << ", " << mom.y()
            << ", " << mom.z() << ") from ";
  if (aStep->GetPreStepPoint()->GetPhysicalVolume()) std::cout << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  else std::cout << "noVolume";
  std::cout << " to ";
  if (aStep->GetPostStepPoint()->GetPhysicalVolume()) std::cout << aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
  else std::cout << "outOfWorld";
  std::cout << " with KE=" << tr->GetKineticEnergy() << " pT=" << tr->GetMomentum().perp()
            << " eta=" << tr->GetMomentum().eta() << " length " << aStep->GetStepLength() << " energy "
            << aStep->GetTotalEnergyDeposit() << " with process ";
  if (aStep->GetPostStepPoint()->GetProcessDefinedStep()) std::cout << aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  else std::cout << "Unknown";
  std::cout << " from a ";
  if (tr->GetCreatorProcess()) std::cout << tr->GetCreatorProcess()->GetProcessName();
  else if (tr->GetParentID()==0) std::cout << "Generator";
  else std::cout << "pid="<<tr->GetParentID();
  if (tr->GetLogicalVolumeAtVertex()) std::cout << " in " << tr->GetLogicalVolumeAtVertex()->GetName() << ", ";
  else std::cout << " nowhere, ";
  std::cout << tr->GetTrackLength() << " mm ago" << std::endl;
  ATH_MSG_WARNING( "HYPERSPACE PARTICLE DETECTED!! KILLING!!" );

  const G4TrackVector *tv = aStep->GetSecondary();
  for (auto atv : *tv ){
    if ( m_world->Inside( atv->GetPosition() ) == kOutside ){
      atv->SetTrackStatus(fStopAndKill);
      ++m_killCount;
      ATH_MSG_WARNING("Had to kill a secondary as well");
    }
  }

  aStep->GetTrack()->SetTrackStatus(fStopAndKill);
  ++m_killCount;

  if (m_killAfter>0 && m_killCount>m_killAfter){
    ATH_MSG_FATAL( m_killAfter << " cases of hyperspace bug found.  You have something seriously wrong going on here.  Needs to be fixed.  Bailing out." );
    throw "OhMyGodWhatWasThat";
  }
}



StatusCode HyperspaceCatcher::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}


/*

  void HyperspaceCatcher::PreUserTrackingAction(const G4Track* aTrack){

  // First time initialization
  if (0==m_world) initAndSetup();

  bool hs = false;

  // Various levels of testing.  Simple testing at level 0 : just see if it's in the neighborhood
  G4ThreeVector myPos = aTrack->GetPosition();
  if (std::fabs( myPos.x() )>30000. ||
  std::fabs( myPos.y() )>30000. ||
  std::fabs( myPos.z() )>50000. ){
  hs=true;
  } // Check #1 failed
  if (!hs && m_treatmentLevel<=0) return;

  if ( m_world->Inside( myPos ) == kOutside ){
  hs=true;
  } // Check #2 failed
  if (!hs && m_treatmentLevel<=1) return;

  G4ThreeVector mom = aTrack->GetMomentumDirection();

  std::cout << "Caught pre-tracking " << aTrack->GetDefinition()->GetParticleName() << " at (" << myPos.x()
  << ", " << myPos.y() << ", " << myPos.z() << ") to (" << mom.x() << ", " << mom.y()
  << ", " << mom.z() << ") ";
  std::cout << " with KE=" << aTrack->GetKineticEnergy() << " pT=" << aTrack->GetMomentum().perp()
  << " eta=" << aTrack->GetMomentum().eta();
  std::cout << " from a ";
  if (aTrack->GetCreatorProcess()) std::cout << aTrack->GetCreatorProcess()->GetProcessName();
  else if (aTrack->GetParentID()==0) std::cout << "Generator";
  else std::cout << "pid="<<aTrack->GetParentID();
  if (aTrack->GetLogicalVolumeAtVertex()) std::cout << " in " << aTrack->GetLogicalVolumeAtVertex()->GetName() << ", ";
  else std::cout << " nowhere, ";
  std::cout << aTrack->GetTrackLength() << " mm ago" << std::endl;
  ATH_MSG_WARNING( "HYPERSPACE PARTICLE DETECTED!! KILLING!!" );

  //  Passed a const track - can't really kill at this point :-(
  //  aTrack->SetTrackStatus(fStopAndKill);
  m_killCount++;
  if (m_killAfter>0 && m_killCount>m_killAfter){
  ATH_MSG_FATAL( m_killAfter << " cases of hyperspace bug found.  You have something seriously wrong going on here.  Needs to be fixed.  Bailing out." );
  throw "OhMyGodWhatWasThat";
  }

  }

*/
