/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TBPhaseStreamerTool.h"
#include "TBEvent/TBPhase.h"
#include <string>
#include <ios>

TBPhaseStreamerTool::TBPhaseStreamerTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
  : TBEventStreamerTool(type,name,parent)
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
