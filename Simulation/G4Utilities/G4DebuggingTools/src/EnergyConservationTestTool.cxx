/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EnergyConservationTestTool.h"

namespace G4UA
{

  EnergyConservationTestTool::EnergyConservationTestTool(const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent)
    : UserActionToolBase<EnergyConservationTest>(type, name, parent)
  {}

  std::unique_ptr<EnergyConservationTest>
  EnergyConservationTestTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing an EnergyConservationTest action");
    auto action = std::make_unique<EnergyConservationTest>();
    actionList.eventActions.push_back( action.get() );
    actionList.trackingActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
