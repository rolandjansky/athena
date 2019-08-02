/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VerboseSelectorTool.h"

namespace G4UA
{

  VerboseSelectorTool::VerboseSelectorTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : UserActionToolBase<VerboseSelector>(type, name, parent)
  {
    declareProperty("TargetEvent",m_config.targetEvent);
    declareProperty("TargetTrack",m_config.targetTrack);
    //declareProperty("TargetBarcode",m_config.targetBarcode);
    declareProperty("VerboseLevel",m_config.verboseLevel);
    declareProperty("Continue",m_config.verb);
    // properties for area check at step time
    declareProperty("Xmin",m_config.Xmin);
    declareProperty("Xmax",m_config.Xmax);

    declareProperty("Ymin",m_config.Ymin);
    declareProperty("Ymax",m_config.Ymax);

    declareProperty("Zmin",m_config.Zmin);
    declareProperty("Zmax",m_config.Zmax);

    // target pdgIDs of tracks to printout
    declareProperty("TargetPdgIDs", m_config.targetPdgIDs);
  }

  std::unique_ptr<VerboseSelector>
  VerboseSelectorTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a VerboseSelector");
    auto action = std::make_unique<VerboseSelector>(m_config);
    actionList.eventActions.push_back( action.get() );
    actionList.trackingActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
