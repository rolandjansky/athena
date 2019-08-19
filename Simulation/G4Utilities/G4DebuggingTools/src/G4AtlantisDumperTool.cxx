/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlantisDumperTool.h"

namespace G4UA
{

  G4AtlantisDumperTool::G4AtlantisDumperTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : UserActionToolBase<G4AtlantisDumper>(type, name, parent)
  {
    declareProperty("Edep", m_config.tedep_cut);
    declareProperty("Length", m_config.length_cut);
    declareProperty("P", m_config.p_cut);
    declareProperty("KE", m_config.ke_cut);
  }

  std::unique_ptr<G4AtlantisDumper>
  G4AtlantisDumperTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a G4AtlasDumper action");
    auto action = std::make_unique<G4AtlantisDumper>(m_config);
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
