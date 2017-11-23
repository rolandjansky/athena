/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/ScoringVolumeTrackKillerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  ScoringVolumeTrackKillerTool::ScoringVolumeTrackKillerTool(const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent)
    : ActionToolBase<ScoringVolumeTrackKiller>(type, name, parent)
  {
    declareInterface<IG4EventActionTool>(this);
    declareInterface<IG4SteppingActionTool>(this);
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<ScoringVolumeTrackKiller>
  ScoringVolumeTrackKillerTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<ScoringVolumeTrackKiller>();
  }

} // namespace G4UA
