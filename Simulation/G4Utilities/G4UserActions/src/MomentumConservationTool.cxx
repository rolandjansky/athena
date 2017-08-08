/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/MomentumConservationTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  MomentumConservationTool::MomentumConservationTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent)
    : ActionToolBase<MomentumConservation>(type, name, parent)
  {
    declareInterface<IEndEventActionTool>(this);
    declareInterface<ISteppingActionTool>(this);
  }

  //---------------------------------------------------------------------------
  std::unique_ptr<MomentumConservation>  MomentumConservationTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<MomentumConservation>();
    return std::move(action);
  }

} // namespace G4UA
