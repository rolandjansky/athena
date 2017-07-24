/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "AthenaStackingActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  AthenaStackingActionTool::
  AthenaStackingActionTool(const std::string& type, const std::string& name,
                           const IInterface* parent)
    : ActionToolBase<AthenaStackingAction>(type, name, parent),
      m_config { /*killAllNeutrinos*/ false, /*photonEnergyCut*/ -1., /*isISFJob*/ false}
  {
    declareProperty("KillAllNeutrinos", m_config.killAllNeutrinos,
                    "Toggle killing of all neutrinos");
    declareProperty("PhotonEnergyCut", m_config.photonEnergyCut,
                    "Energy threshold for tracking photons");
    declareProperty("IsISFJob", m_config.isISFJob, "");
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode AthenaStackingActionTool::initialize()
  {
    ATH_MSG_DEBUG("initialize");
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<AthenaStackingAction>
  AthenaStackingActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    // Create and configure the action plugin.
    auto action = CxxUtils::make_unique<AthenaStackingAction>(m_config);
    return std::move(action);
  }

  //---------------------------------------------------------------------------
  // Query interface
  //---------------------------------------------------------------------------
  StatusCode AthenaStackingActionTool::queryInterface(const InterfaceID& riid,
                                                      void** ppvIf)
  {
    if(riid == IStackingActionTool::interfaceID()) {
      *ppvIf = (IStackingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<AthenaStackingAction>::queryInterface(riid, ppvIf);
  }

}
