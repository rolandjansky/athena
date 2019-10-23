// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CutBookkeeperUtils/OriginalAodCountsTool.h"
#include "CutBookkeeperUtils/OriginalAodCounts.h"
#include "AsgMessaging/StatusCode.h"
#include "AsgMessaging/MsgStreamMacros.h"

OriginalAodCountsTool::OriginalAodCountsTool(const std::string& name):
  asg::AsgTool(name),
  m_config()
{
}
OriginalAodCountsTool::~OriginalAodCountsTool(){
}

StatusCode OriginalAodCountsTool::initialize() {
  // read in configuration stuff here
  return StatusCode::SUCCESS;
}

StatusCode OriginalAodCountsTool::incrementAodCounts(
  xAOD::TEvent& event,
  OriginalAodCounts& counts) const {
  try {
    counts += getOriginalAodCounts(event, m_config);
    return StatusCode::SUCCESS;
  } catch (std::runtime_error& e) {
    ATH_MSG_FATAL("issue getting aod counts: " << e.what());
    return StatusCode::FAILURE;
  }
}
