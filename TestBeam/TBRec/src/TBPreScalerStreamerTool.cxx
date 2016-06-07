/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBRec/TBEventStreamerTool.h"
#include "TBRec/TBPreScalerStreamerTool.h"

// already included in TBPreScalerStreamerTool.h
//#include "CLHEP/Random/RanluxEngine.h"

#include <string>

using CLHEP::RanluxEngine;

TBPreScalerStreamerTool::TBPreScalerStreamerTool(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  : TBEventStreamerTool(type,name,parent)
    , m_acceptFraction(1.)
    , m_randomizer(nullptr)
{
  declareInterface<TBEventStreamerTool>(this); 
  declareProperty("AcceptEventFraction",m_acceptFraction);
}

TBPreScalerStreamerTool::~TBPreScalerStreamerTool()
{ }

StatusCode TBPreScalerStreamerTool::initializeTool()
{
  // check configuration
  if ( m_acceptFraction < 0 || m_acceptFraction > 1 )
    {
      MsgStream report(msgSvc(),name());
      report << MSG::ERROR
	     << "event accept frequency outside allowed range [0,1]"
	     << endreq;
      return StatusCode::FAILURE;
    }
  // initialize randomizer
  m_randomizer = new RanluxEngine();
  return m_randomizer != 0
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}

StatusCode TBPreScalerStreamerTool::accept()
{
  return m_randomizer->flat() < m_acceptFraction
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}
