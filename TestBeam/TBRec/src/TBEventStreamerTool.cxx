/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBRec/TBEventStreamerTool.h"

#include <string>

TBEventStreamerTool::TBEventStreamerTool(const std::string& name,
					 const std::string& type,
					 const IInterface* parent)
  : AlgTool(name,type,parent),
    m_storeGate(0)
{ }

TBEventStreamerTool::~TBEventStreamerTool()
{ }

StatusCode TBEventStreamerTool::initialize()
{
  MsgStream report(msgSvc(),name());

  StatusCode checkOut = service("StoreGateSvc",m_storeGate);
  if ( checkOut.isFailure() )
    {
      report << MSG::ERROR
	     << "Cannot alllocate StoreGate service!"
	     << endreq;
    }

  this->initializeTool();

  return checkOut;
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
