/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEventStreamerTool.h"
#include <string>

TBEventStreamerTool::TBEventStreamerTool(const std::string& name,
					 const std::string& type,
					 const IInterface* parent)
  : AthAlgTool(name,type,parent)
{ }

TBEventStreamerTool::~TBEventStreamerTool()
{ }

StatusCode TBEventStreamerTool::initialize()
{
  return this->initializeTool();
}

StatusCode TBEventStreamerTool::initializeTool()
{
  return StatusCode::SUCCESS;
}

StatusCode TBEventStreamerTool::reject()
{
  return this->accept().isFailure()
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}
