/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloG4Sim/CalibrationDefaultProcessingTool.h"

namespace G4UA
{

  namespace CaloG4
  {

    CalibrationDefaultProcessingTool::
    CalibrationDefaultProcessingTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
      : UserActionToolBase<CalibrationDefaultProcessing>(type, name, parent)
    {
      declareProperty("SDName", m_config.SDName);
    }

    std::unique_ptr<CalibrationDefaultProcessing>
    CalibrationDefaultProcessingTool::makeAndFillAction(G4AtlasUserActions& actionList)
    {
      ATH_MSG_DEBUG("Constructing a CalibrationDefaultProcessing action");
      auto action = std::make_unique<CalibrationDefaultProcessing>(m_config);
      actionList.eventActions.push_back( action.get() );
      actionList.steppingActions.push_back( action.get() );
      return action;
    }

  } // namespace CaloG4

} // namespace G4UA
