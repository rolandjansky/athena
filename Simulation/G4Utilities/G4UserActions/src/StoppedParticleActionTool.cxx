/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/StoppedParticleActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  StoppedParticleActionTool::StoppedParticleActionTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent)
    : ActionToolBase<StoppedParticleAction>(type, name, parent)
  {
    declareProperty("StoppingCondition",m_stoppingCondition=-3,"Negative ignores gasses; number indicates condition (effectively an enum)");
    declareInterface<IG4SteppingActionTool>(this);
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<StoppedParticleAction> StoppedParticleActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return std::make_unique<StoppedParticleAction>(m_stoppingCondition);
  }

} // namespace G4UA
