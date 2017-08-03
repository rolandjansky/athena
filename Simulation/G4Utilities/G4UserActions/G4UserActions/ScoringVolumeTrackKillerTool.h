/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__SCORINGVOLUMETRACKKILLERTOOL_H
#define G4USERACTIONS_G4UA__SCORINGVOLUMETRACKKILLERTOOL_H
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/ScoringVolumeTrackKiller.h"

namespace G4UA
{

  /// @brief Tool which manages the ScoringVolumeTrackKiller user action.
  ///
  class ScoringVolumeTrackKillerTool : public ActionToolBase<ScoringVolumeTrackKiller>,
                                       public IG4EventActionTool,
                                       public IG4SteppingActionTool
  {

    public:

      /// Standard constructor
      ScoringVolumeTrackKillerTool(const std::string& type, const std::string& name,
                                   const IInterface* parent);

      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

    protected:

      virtual std::unique_ptr<ScoringVolumeTrackKiller> makeAction() override final;

  }; // class ScoringVolumeTrackKillerTool

} // namespace G4UA

#endif
