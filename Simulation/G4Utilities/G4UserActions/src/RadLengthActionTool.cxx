/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/RadLengthActionTool.h"

namespace G4UA{


  RadLengthActionTool::RadLengthActionTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : ActionToolBase<RadLengthAction>(type, name, parent)
    , m_config()
  {
    declareInterface<IG4RunActionTool>(this);
    declareInterface<IG4EventActionTool>(this);
    declareInterface<IG4SteppingActionTool>(this);
    declareProperty("VolumeDepthLevel",m_config.VolumeDepthLevel);
  }

  std::unique_ptr<RadLengthAction>  RadLengthActionTool::makeAction()
  {
    ATH_MSG_DEBUG("makeAction");
    return std::make_unique<RadLengthAction>(m_config);
  }

} // namespace G4UA
