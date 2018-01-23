/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4UserActions/RadiationMapsMakerTool.h"

namespace G4UA{ 

  RadiationMapsMakerTool::RadiationMapsMakerTool(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
    : ActionToolBase<RadiationMapsMaker>(type, name, parent)
  {
    declareInterface<IG4RunActionTool>(this);
    declareInterface<IG4EventActionTool>(this);
    declareInterface<IG4SteppingActionTool>(this);
  }

  std::unique_ptr<RadiationMapsMaker>  RadiationMapsMakerTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    return CxxUtils::make_unique<RadiationMapsMaker>();
  }

} // namespace G4UA 
