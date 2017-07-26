/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
    return std::make_unique<MomentumConservation>();
  }

} // namespace G4UA
