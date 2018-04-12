/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_HIPKiller_H
#define G4UserActions_HIPKiller_H

#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @brief Kills Monopoles and QBalls with energy < 1 MeV
  class HIPKiller : public G4UserSteppingAction, public AthMessaging
  {
    public:
      HIPKiller();
      virtual void UserSteppingAction(const G4Step*) override;
    private:
  }; // class HIPKiller

} // namespace G4UA

#endif
