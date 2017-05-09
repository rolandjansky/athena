/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_HIPKiller_H
#define G4UserActions_HIPKiller_H

#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @brief Kills Monopoles and QBalls with energy < 1 MeV
  class HIPKiller : public ISteppingAction, public AthMessaging
  {
    public:
      HIPKiller();
      virtual void processStep(const G4Step*) override;
    private:
  }; // class HIPKiller

} // namespace G4UA

#endif
