/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RadLenNtupleTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  RadLenNtupleTool::RadLenNtupleTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : UserActionToolBase<RadLenNtuple>(type, name, parent)
  {
    declareProperty("McEventCollName", m_config.mcEventCollName);
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode RadLenNtupleTool::initialize()
  {
    if(msgLvl(MSG::VERBOSE))    { m_config.verboseLevel = 10; }
    else if(msgLvl(MSG::DEBUG)) { m_config.verboseLevel = 5;  }
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<RadLenNtuple>
  RadLenNtupleTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a RadLenNtuple action");
    auto action = std::make_unique<RadLenNtuple>(m_config);
    if(action->initialize().isFailure()) {
      ATH_MSG_WARNING("Failed to set up RadLenNtuple properly!");
    }
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

}
