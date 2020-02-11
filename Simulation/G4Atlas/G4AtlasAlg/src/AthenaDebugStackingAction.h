/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4UA_ATHENADEBUGSTACKINGACTION_H
#define G4ATLASALG_G4UA_ATHENADEBUGSTACKINGACTION_H

// Base class
#include "AthenaStackingAction.h"


namespace G4UA
{

  /// @class AthenaDebugStackingAction
  /// @brief Debug version of the AthenaStackingAction
  /// used for validation of Russian Roulette algorimts.
  /// It can ensure that there is no randomization in
  /// simulation caused by turning the Russian Roulette
  /// on or off.
  ///
  /// @author Miha Muskinja <Miha.Muskinja@cern.ch>
  ///
  class AthenaDebugStackingAction : public AthenaStackingAction
  {

    public:

      /// Constructor with configuration
      AthenaDebugStackingAction(const Config& config);

      /// @brief Classify a new track.
      /// Result can be fUrgent, fWaiting, fPostpone, or fKill.
      virtual G4ClassificationOfNewTrack
      ClassifyNewTrack(const G4Track* track) override final;

  }; // class AthenaDebugStackingAction

} // namespace G4UA

#endif
