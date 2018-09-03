/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PrescaleTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// Based on the Integrated Simulation Framework
//
// Use of ExpressionParsing to analyse a more complex string

#include "DerivationFrameworkTools/PrescaleTool.h"
#include "xAODEventInfo/EventInfo.h"

namespace DerivationFramework {

  PrescaleTool::PrescaleTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_prescale(1)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("Prescale", m_prescale);
  }

  StatusCode PrescaleTool::initialize()
  {
    if (m_prescale < 1) {
      ATH_MSG_FATAL("Prescale of less than 1 makes no sense");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode PrescaleTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  bool PrescaleTool::eventPassesFilter() const
  {
    const xAOD::EventInfo* ei = 0;
    CHECK( evtStore()->retrieve( ei , "EventInfo" ) );
    bool accept(false);
    if (ei->eventNumber() % m_prescale == 0) accept = true;
    return accept;
  }  

}
