/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_ScoringVolumeTrackKiller_H
#define G4UserActions_ScoringVolumeTrackKiller_H

#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class ScoringVolumeTrackKiller : public AthMessaging, public IEndEventAction,
                                   public ISteppingAction
  {
    public:
      ScoringVolumeTrackKiller();
      virtual void endOfEvent(const G4Event*) override;
      virtual void processStep(const G4Step*) override;
    private:
      unsigned long m_killCount;
  }; // class ScoringVolumeTrackKiller

} // namespace G4UA

#endif
