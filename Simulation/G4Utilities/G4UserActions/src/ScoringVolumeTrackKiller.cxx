/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "G4UserActions/ScoringVolumeTrackKiller.h"

#include <string>
#include <iostream>

#include "G4Step.hh"
#include "G4TrackVector.hh"


static ScoringVolumeTrackKiller svtk("ScoringVolumeTrackKiller");


void ScoringVolumeTrackKiller::BeginOfEventAction(const G4Event*)
{
  killCount = 0;
}


void ScoringVolumeTrackKiller::EndOfEventAction(const G4Event*)
{
  log() << MSG::INFO << killCount << " tracks killed in this event " << endreq;
}


void ScoringVolumeTrackKiller::BeginOfRunAction(const G4Run*)
{
  log() << MSG::INFO << " ScoringVolumeTrackKiller kills all particles leaving the"
            << " envelope of a muon station " << endreq;
}


void ScoringVolumeTrackKiller::EndOfRunAction(const G4Run*)
{
  log() << MSG::INFO << " End of run for ScoringVolumeTrackKiller: " 
            << killCount << " tracks killed during this run" 
            << endreq;
}


void ScoringVolumeTrackKiller::SteppingAction(const G4Step* aStep)
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
