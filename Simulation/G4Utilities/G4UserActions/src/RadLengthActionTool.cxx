/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RadLengthActionTool.h"

namespace G4UA{


  RadLengthActionTool::RadLengthActionTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : UserActionToolBase<RadLengthAction>(type, name, parent)
    , m_config()
  {
    declareProperty("VolumeDepthLevel",m_config.VolumeDepthLevel);
  }

  std::unique_ptr<RadLengthAction>
  RadLengthActionTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a RadLengthAction action");
    auto action = std::make_unique<RadLengthAction>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
