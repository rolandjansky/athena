/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__SCORINGVOLUMETRACKKILLERTOOL_H
#define G4USERACTIONS_G4UA__SCORINGVOLUMETRACKKILLERTOOL_H

#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"
#include "G4UserActions/ScoringVolumeTrackKiller.h"

namespace G4UA
{

  /// @brief Tool which manages the ScoringVolumeTrackKiller user action.
  ///
  class ScoringVolumeTrackKillerTool
    : public UserActionToolBase<ScoringVolumeTrackKiller>
  {

    public:

      /// Standard constructor
      ScoringVolumeTrackKillerTool(const std::string& type, const std::string& name,
                                   const IInterface* parent);

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<ScoringVolumeTrackKiller>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class ScoringVolumeTrackKillerTool

} // namespace G4UA

#endif
