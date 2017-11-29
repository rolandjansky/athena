/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
    declareInterface<IG4StackingActionTool>(this);
    declareProperty("KillAllNeutrinos", m_config.killAllNeutrinos,
                    "Toggle killing of all neutrinos");
    declareProperty("PhotonEnergyCut", m_config.photonEnergyCut,
                    "Energy threshold for tracking photons");
    declareProperty("IsISFJob", m_config.isISFJob, "");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode AthenaStackingActionTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<AthenaStackingAction>
  AthenaStackingActionTool::makeAction()
  {
    ATH_MSG_DEBUG("Creating an AthenaStackingAction");
    // Create and configure the action plugin.
    return std::make_unique<AthenaStackingAction>(m_config);
  }

}
