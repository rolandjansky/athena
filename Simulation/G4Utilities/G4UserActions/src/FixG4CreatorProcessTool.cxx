/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FixG4CreatorProcessTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  FixG4CreatorProcessTool::FixG4CreatorProcessTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : UserActionToolBase<FixG4CreatorProcess>(type, name, parent)
  {
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<FixG4CreatorProcess>
  FixG4CreatorProcessTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a FixG4CreatorProcess action");
    auto action = std::make_unique<FixG4CreatorProcess>();
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
