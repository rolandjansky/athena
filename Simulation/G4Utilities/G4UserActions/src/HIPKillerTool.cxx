/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/HIPKillerTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  HIPKillerTool::HIPKillerTool(const std::string& type, const std::string& name,
                               const IInterface* parent)
    : ActionToolBase<HIPKiller>(type, name, parent)
  {
    declareInterface<IG4SteppingActionTool>(this);
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<HIPKiller>  HIPKillerTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<HIPKiller>();
    return std::move(action);
  }

} // namespace G4UA
