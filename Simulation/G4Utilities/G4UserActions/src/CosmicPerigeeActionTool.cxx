/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/CosmicPerigeeActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CosmicPerigeeActionTool::CosmicPerigeeActionTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
    : ActionToolBase<CosmicPerigeeAction>(type, name, parent)
  {
    declareInterface<IG4SteppingActionTool>(this);
    declareInterface<IG4EventActionTool>(this);
    declareInterface<IG4TrackingActionTool>(this);
    declareProperty("pMinPrimary", m_config.pMinPrimary);
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<CosmicPerigeeAction> CosmicPerigeeActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return std::make_unique<CosmicPerigeeAction>(m_config);
  }

}
