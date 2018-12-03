/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_ScoringVolumeTrackKiller_H
#define G4UserActions_ScoringVolumeTrackKiller_H

#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class ScoringVolumeTrackKiller : public AthMessaging, public G4UserEventAction,
                                   public G4UserSteppingAction
  {
    public:
      ScoringVolumeTrackKiller();
      virtual void EndOfEventAction(const G4Event*) override;
      virtual void UserSteppingAction(const G4Step*) override;
    private:
      unsigned long m_killCount;
  }; // class ScoringVolumeTrackKiller

} // namespace G4UA

#endif
