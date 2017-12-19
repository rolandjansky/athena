/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthSteppingActionTool.h"
#include "CxxUtils/make_unique.h"


namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  MCTruthSteppingActionTool::
  MCTruthSteppingActionTool(const std::string& type, const std::string& name,
                            const IInterface* parent)
    : UserActionToolBase<MCTruthSteppingAction>(type, name, parent)
  {
    declareProperty("VolumeCollectionMap", m_volumeCollectionMap,
                    "Map of volume name to output collection name");
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode MCTruthSteppingActionTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create an MCTruthSteppingAction
  //---------------------------------------------------------------------------
  std::unique_ptr<MCTruthSteppingAction>
  MCTruthSteppingActionTool::makeAndFillAction(G4AtlasUserActions& actionLists)
  {
    ATH_MSG_DEBUG("Constructing an MCTruthSteppingAction");
    auto action = std::make_unique<MCTruthSteppingAction> (
        m_volumeCollectionMap, msgSvc(), msg().level() );
    actionLists.eventActions.push_back( action.get() );
    actionLists.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
