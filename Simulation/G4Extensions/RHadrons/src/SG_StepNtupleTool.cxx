/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SG_StepNtupleTool.h"

namespace G4UA
{

  SG_StepNtupleTool::SG_StepNtupleTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent)
    : UserActionToolBase<SG_StepNtuple>(type, name, parent)
  {
    declareProperty("RHadronPDGIDList",m_pdgids,"List of R-Hadron PDG IDs");
  }

  std::unique_ptr<SG_StepNtuple>
  SG_StepNtupleTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing an SG_StepNtuple action");
    auto action = std::make_unique<SG_StepNtuple>(m_pdgids);
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
