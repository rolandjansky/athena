/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__SCORINGVOLUMETRACKKILLERTOOL_H
#define G4USERACTIONS_G4UA__SCORINGVOLUMETRACKKILLERTOOL_H
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/ScoringVolumeTrackKiller.h"

namespace G4UA
{

  /// @brief Tool which manages the ScoringVolumeTrackKiller user action.
  ///
  class ScoringVolumeTrackKillerTool : public ActionToolBase<ScoringVolumeTrackKiller>,
                                       public IEndEventActionTool,
                                       public ISteppingActionTool
  {

    public:

      /// Standard constructor
      ScoringVolumeTrackKillerTool(const std::string& type, const std::string& name,
                                   const IInterface* parent);

      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }
      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

    protected:

      virtual std::unique_ptr<ScoringVolumeTrackKiller> makeAction() override final;

  }; // class ScoringVolumeTrackKillerTool

} // namespace G4UA

#endif
