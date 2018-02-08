/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/ScoringVolumeTrackKiller.h"

#include <string>
#include <iostream>

#include "G4Step.hh"
#include "G4TrackVector.hh"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  ScoringVolumeTrackKiller::ScoringVolumeTrackKiller()
    : AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),
                   "ScoringVolumeTrackKiller")
    , m_killCount(0)
  {}

  //---------------------------------------------------------------------------
  void ScoringVolumeTrackKiller::EndOfEventAction(const G4Event*)
  {
    ATH_MSG_INFO( m_killCount << " tracks killed in this event " );
    m_killCount = 0;
  }

  //---------------------------------------------------------------------------
  void ScoringVolumeTrackKiller::UserSteppingAction(const G4Step* aStep)
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
      m_killCount++;
    }
  }

} // namespace G4UA
