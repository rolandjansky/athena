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
      StoppedParticleAction();
      virtual void UserSteppingAction(const G4Step*) override;
    private:
      bool isSUSYParticle(const int) const;
      TrackFastSimSD * m_fsSD;
      bool m_init;
  }; // class StoppedParticleAction

} // namespace G4UA

#endif
