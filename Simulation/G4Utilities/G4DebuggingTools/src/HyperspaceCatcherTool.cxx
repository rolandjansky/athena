/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HyperspaceCatcherTool.h"

namespace G4UA
{

  HyperspaceCatcherTool::HyperspaceCatcherTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent)
    : UserActionToolBase<HyperspaceCatcher>(type, name, parent)
  {
    declareProperty("TreatmentLevel", m_config.treatmentLevel);
    declareProperty("KillAfter", m_config.killAfter);
  }

  std::unique_ptr<HyperspaceCatcher>
  HyperspaceCatcherTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a HyperspaceCatcher");
    auto action = std::make_unique<HyperspaceCatcher>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
