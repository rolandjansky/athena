/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_MomentumConservation_H
#define G4UserActions_MomentumConservation_H

#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

#include <iostream>

namespace G4UA
{

  /// @class MomentumConservation
  /// @brief checks momentum conservation
  class MomentumConservation : public G4UserEventAction,
                               public G4UserSteppingAction,
                               public AthMessaging
  {
    public:

      MomentumConservation();

      virtual void EndOfEventAction(const G4Event*) override;
      virtual void UserSteppingAction(const G4Step*) override;

    private:

      /// Sum of energy deposited in the detector
      double _sum_edep;

      /// Sum of energy escaping the detector (volume = 0)
      double _sum_eesc;

  }; // class MomentumConservation

} // namespace G4UA

#endif
