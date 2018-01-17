/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/ScoringVolumeTrackKillerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  ScoringVolumeTrackKillerTool::ScoringVolumeTrackKillerTool(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
    : UserActionToolBase<ScoringVolumeTrackKiller>(type, name, parent)
  {
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<ScoringVolumeTrackKiller>
  ScoringVolumeTrackKillerTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a ScoringVolumeTrackKiller action");
    auto action = std::make_unique<ScoringVolumeTrackKiller>();
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
