/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/FluxRecorderTool.h"

namespace G4UA
{

  FluxRecorderTool::FluxRecorderTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : ActionToolBase<FluxRecorder>(type, name, parent)
  {
    declareInterface<IBeginRunActionTool>(this);
    declareInterface<IEndRunActionTool>(this);
    declareInterface<IEndEventActionTool>(this);
    declareInterface<ISteppingActionTool>(this);
  }

  std::unique_ptr<FluxRecorder> FluxRecorderTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<FluxRecorder>();
  }

} // namespace G4UA
