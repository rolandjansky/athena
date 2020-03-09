/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__SCORINGVOLUMETRACKKILLERTOOL_H
#define G4USERACTIONS_G4UA__SCORINGVOLUMETRACKKILLERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "ScoringVolumeTrackKiller.h"

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
