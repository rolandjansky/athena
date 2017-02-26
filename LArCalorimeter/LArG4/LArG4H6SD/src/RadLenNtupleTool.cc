/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "RadLenNtupleTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  RadLenNtupleTool::RadLenNtupleTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : ActionToolBase<RadLenNtuple>(type, name, parent)
  {
    declareProperty("McEventCollName", m_config.mcEventCollName);
  }

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
  RadLenNtupleTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<RadLenNtuple>(m_config);
    if(action->initialize().isFailure())
      {
        ATH_MSG_WARNING("Failed to set up RadLenNtuple properly!");
      }
    return std::move(action);
  }

  //---------------------------------------------------------------------------
  // Query interface
  //---------------------------------------------------------------------------
  StatusCode RadLenNtupleTool::queryInterface(const InterfaceID& riid, void** ppvIf)
  {
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }

    if(riid == IEndEventActionTool::interfaceID()) {
      *ppvIf = (IEndEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }

    if(riid == IBeginEventActionTool::interfaceID()) {
      *ppvIf = (IBeginEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }

    return ActionToolBase<RadLenNtuple>::queryInterface(riid, ppvIf);
  }

}
