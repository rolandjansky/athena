/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "G4UserActions/ScoringVolumeTrackKiller.h"

#include <string>
#include <iostream>

#include "G4Step.hh"
#include "G4TrackVector.hh"


void ScoringVolumeTrackKiller::EndOfEvent(const G4Event*)
{
  ATH_MSG_INFO( killCount << " tracks killed in this event " );
  killCount = 0;
}

void ScoringVolumeTrackKiller::Step(const G4Step* aStep)
{
  G4StepPoint* preStep = aStep->GetPreStepPoint();
  const G4VTouchable* preTouchable = preStep->GetTouchable();
  G4StepPoint* postStep = aStep->GetPostStepPoint();
  const G4VTouchable* postTouchable = postStep->GetTouchable();
  int preDepth = preTouchable->GetHistoryDepth();
  int postDepth = postTouchable->GetHistoryDepth();
  
  std::string preName;
  bool preInStation = false;
  for (int i = 0; i < preDepth+1; i++) {
    preName = preTouchable->GetVolume(i)->GetName();
    if (preName.find("station") != std::string::npos ||
        preName.find("av_") != std::string::npos) {

      preInStation = true;
      break;
    }
  }
  std::string postName;
  bool postOutofStation = true;
  bool outOfMother = false;
  for (int i = 0; i < postDepth+1; i++) {
    if (postTouchable->GetVolume(i) != 0) {
      postName = postTouchable->GetVolume(i)->GetName();
    } else {
      outOfMother = true;
      break;
    }
    if (postName.find("station") != std::string::npos ||
        postName.find("av_") != std::string::npos) {

      postOutofStation = false;
      break;
    }
  }
  if ( (preInStation && postOutofStation) || outOfMother ) {
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
//    std::cout << aStep->GetTrack()->GetDefinition()->GetParticleName() << " stopped " << std::endl;
//    double x = aStep->GetTrack()->GetPosition().x();
//    double y = aStep->GetTrack()->GetPosition().y();
//    double z = aStep->GetTrack()->GetPosition().z();
//    double r = std::sqrt(x*x + y*y);
//    std::cout << " at r = " << r << ", z = " << z << std::endl;
    killCount++;
//    G4TrackVector* secVec = aStep->GetSecondary();
//    std::cout << secVec->size() << " secondaries " << std::endl;
//    std::cout << " Track " << trackID << " killed at pre = " 
//              << preName << " and post = " << postName << std::endl;
  }
}

StatusCode ScoringVolumeTrackKiller::queryInterface(const InterfaceID& riid, void** ppvInterface) 
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
