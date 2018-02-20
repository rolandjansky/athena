/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/FluxRecorderTool.h"

namespace G4UA
{

  FluxRecorderTool::FluxRecorderTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : ActionToolBase<FluxRecorder>(type, name, parent)
  {
    declareInterface<IG4RunActionTool>(this);
    declareInterface<IG4EventActionTool>(this);
    declareInterface<IG4SteppingActionTool>(this);
  }

  std::unique_ptr<FluxRecorder> FluxRecorderTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return std::make_unique<FluxRecorder>();
  }

} // namespace G4UA
