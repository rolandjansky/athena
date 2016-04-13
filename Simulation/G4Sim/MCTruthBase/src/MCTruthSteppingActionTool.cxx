/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "MCTruthSteppingActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  MCTruthSteppingActionTool::
  MCTruthSteppingActionTool(const std::string& type, const std::string& name,
                            const IInterface* parent)
    : ActionToolBase<MCTruthSteppingAction>(type, name, parent)
  {
    declareInterface<ISteppingActionTool>(this);
    declareInterface<IBeginEventActionTool>(this);
    declareProperty("VolumeCollectionMap", m_volumeCollectionMap,
                    "Map of volume name to output collection name");
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode MCTruthSteppingActionTool::initialize()
  {
    ATH_MSG_DEBUG("initializing MCTruthSteppingActionTool");
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create an MCTruthSteppingAction
  //---------------------------------------------------------------------------
  std::unique_ptr<MCTruthSteppingAction>
  MCTruthSteppingActionTool::makeAction()
  {
    ATH_MSG_DEBUG("Constructing an MCTruthSteppingAction");
    auto action =
      CxxUtils::make_unique<MCTruthSteppingAction>
        ( m_volumeCollectionMap, msgSvc(), msg().level() );
    return std::move(action);
  }

} // namespace G4UA
