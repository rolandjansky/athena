/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HitWrapperTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  HitWrapperTool::HitWrapperTool(const std::string& type, const std::string& name,
                                 const IInterface* parent)
    : UserActionToolBase<HitWrapper>(type, name, parent)
  {
    declareProperty("Time", m_config.time);
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<HitWrapper>
  HitWrapperTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Making a HitWrapper action");
    auto action = std::make_unique<HitWrapper>(m_config);
    actionList.eventActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
