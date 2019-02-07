/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_StoppedParticleAction_h
#define G4UserActions_StoppedParticleAction_h

#include <string>

#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

class TrackFastSimSD;

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class StoppedParticleAction : public AthMessaging, public G4UserSteppingAction
  {
    public:
      StoppedParticleAction(const int condition);
      virtual void UserSteppingAction(const G4Step*) override;
    private:
      TrackFastSimSD * m_fsSD; // Sensitive detector for storing the records of stopped particles
      bool m_init; // Whether the tool has been initialized or not
      int m_stoppingCondition; // Enum describing stopping configuration
  }; // class StoppedParticleAction

} // namespace G4UA

#endif
