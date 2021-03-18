/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Includes from this package:
#include "JetRecTools/JetUsedInFitTrackDecoratorTool.h"

JetUsedInFitTrackDecoratorTool::JetUsedInFitTrackDecoratorTool(const std::string& name):
  asg::AsgTool(name),
  m_decoTool("InDet::InDetUsedInFitTrackDecoratorTool/IDUsedInFitDecoTool_" + name, this)
{
  declareProperty("Decorator", m_decoTool, "InDet::InDetUsedInFitTrackDecoratorTool instance");
}

StatusCode JetUsedInFitTrackDecoratorTool::initialize() {
  ATH_MSG_INFO("Initializing tool " << name() << "...");
  ATH_CHECK(m_decoTool.retrieve());
  return StatusCode::SUCCESS;
}

int JetUsedInFitTrackDecoratorTool::execute() const {
  ATH_MSG_DEBUG("Executing tool " << name() << "...");
  m_decoTool->decorate();
  return 0;
}
