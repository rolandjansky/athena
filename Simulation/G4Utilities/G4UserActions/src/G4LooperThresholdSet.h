/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_LooperThresholdSet_H
#define G4UserActions_LooperThresholdSet_H

#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

#include "G4Run.hh"

namespace G4UA
{

  /// @brief Kills Monopoles and QBalls with energy < 1 MeV
  class LooperThresholdSet : public G4UserSteppingAction, public AthMessaging
  {
    public:
      LooperThresholdSet();
      virtual void BeginOfRunAction(const G4Run* aRun) override;
    private:
      void ChangeLooperParameters(const G4ParticleDefinition* particleDef );
      std::pair<G4Transportation*, G4CoupledTransportation*> findTransportation( const G4ParticleDefinition* particleDef, bool reportError );
  }; // class LooperThresholdSet

} // namespace G4UA

#endif
