/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBRec/TBEventStreamerTool.h"

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
  this->initializeTool();
  return StatusCode::SUCCESS;
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
