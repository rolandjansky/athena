/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
    return std::make_unique<HIPKiller>();
  }

} // namespace G4UA
