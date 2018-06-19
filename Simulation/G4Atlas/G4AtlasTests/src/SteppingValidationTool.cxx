/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SteppingValidationTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  SteppingValidationTool::SteppingValidationTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
    : UserActionToolBase<SteppingValidation>(type, name, parent)
  {}

  //---------------------------------------------------------------------------
  std::unique_ptr<SteppingValidation>
  SteppingValidationTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a SteppingValidation action");
    auto action = std::make_unique<SteppingValidation>();
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
