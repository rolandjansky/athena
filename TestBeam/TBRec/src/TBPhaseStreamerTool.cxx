/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "StoreGate/StoreGateSvc.h"
#include "TBEvent/TBPhase.h"
#include "TBRec/TBPhaseStreamerTool.h"

#include <string>
#include <ios>

TBPhaseStreamerTool::TBPhaseStreamerTool(const std::string& name,
						     const std::string& type,
						     const IInterface* parent)
  : TBEventStreamerTool(name,type,parent)
    , m_guardValue(11.)         // in TDC counts
    , m_TBPhaseKey("TBPhase")
{
  declareInterface<TBEventStreamerTool>(this); 
  declareProperty("TBPhaseKey",m_TBPhaseKey);
  declareProperty("Guard",m_guardValue);
}

TBPhaseStreamerTool::~TBPhaseStreamerTool()
{ }

////////////////
// Initialize //
////////////////

StatusCode TBPhaseStreamerTool::initializeTool()
{ 
  MsgStream report(msgSvc(),name());
  
  report << MSG::INFO << " Cut events using guard value: " << m_guardValue;
  
  return StatusCode::SUCCESS;
}

///////////////////
// Accept/Reject //
///////////////////

StatusCode TBPhaseStreamerTool::accept()
{
  MsgStream report(msgSvc(),name());
  
  const TBPhase* thePhase = 0;
  ATH_CHECK( evtStore()->retrieve(thePhase,m_TBPhaseKey) );

  // Get the absolute value of the distance of the TDC-WAC
  float dTtoWAC = thePhase->getdTtoWAC();

  return (dTtoWAC > m_guardValue)
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}
