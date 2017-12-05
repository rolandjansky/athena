/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/HitWrapperTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  HitWrapperTool::HitWrapperTool(const std::string& type, const std::string& name,
                                 const IInterface* parent)
    : ActionToolBase<HitWrapper>(type, name, parent)
  {
    declareInterface<IG4EventActionTool>(this);
    declareProperty("Time", m_config.time);
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<HitWrapper> HitWrapperTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<HitWrapper>(m_config);
  }

} // namespace G4UA
