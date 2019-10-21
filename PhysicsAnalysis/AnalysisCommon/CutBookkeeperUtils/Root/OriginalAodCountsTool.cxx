// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CutBookkeeperUtils/OriginalAodCountsTool.h"
#include "CutBookkeeperUtils/OriginalAodCounts.h"
#include "AsgMessaging/StatusCode.h"
#include "AsgMessaging/MsgStreamMacros.h"

OriginalAodCountsTool::OriginalAodCountsTool(const std::string& name):
  asg::AsgTool(name)
{
}
OriginalAodCountsTool::~OriginalAodCountsTool() {
}

StatusCode OriginalAodCountsTool::incrementAodCounts(xAOD::TEvent& event, OriginalAodCounts& counts) {
  try {
    counts += getOriginalAodCounts(event);
    return StatusCode::SUCCESS;
  } catch (std::runtime_error& e) {
    ATH_MSG_FATAL("issue getting aod counts: " << e.what());
    return StatusCode::FAILURE;
  }
}
