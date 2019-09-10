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
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
  }

  StatusCode PrescaleTool::initialize()
  {
    if (m_prescale < 1u) {
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
    return (Gaudi::Hive::currentContext().eventID().event_number() % m_prescale == 0);
  }  

}
