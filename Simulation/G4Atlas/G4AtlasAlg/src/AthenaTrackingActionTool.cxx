/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "AthenaTrackingActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  AthenaTrackingActionTool::
  AthenaTrackingActionTool(const std::string& type, const std::string& name,
                           const IInterface* parent)
    : ActionToolBase<AthenaTrackingAction>(type, name, parent)
  {
    declareInterface<IPreTrackingActionTool>(this);
    declareInterface<IPostTrackingActionTool>(this);
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode AthenaTrackingActionTool::initialize()
  {
    ATH_MSG_INFO("initialize");
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<AthenaTrackingAction>
  AthenaTrackingActionTool::makeAction()
  {
    ATH_MSG_DEBUG("Constructing an AthenaTrackingAction");
    // Create and configure the action plugin.
    auto action = CxxUtils::make_unique<AthenaTrackingAction>( msg().level() );
    return std::move(action);
  }

}
