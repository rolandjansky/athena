/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DigitizationAlg.h"

#include "PileUpTools/IPileUpTool.h"

/////////////////////////////////////////////////////////////////////////////

DigitizationAlg::DigitizationAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_puTools()
{
  declareProperty("PileUpTools", m_puTools, "IPileUpTools to be run for each event");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DigitizationAlg::initialize(){
  ATH_MSG_DEBUG ( "Initializing " << name() << " - package version " << PACKAGE_VERSION );
  StatusCode sc(StatusCode::FAILURE);
  //locate the PileupTools and initialize them
  if (!(sc=m_puTools.retrieve()).isSuccess()) {
    ATH_MSG_ERROR ("Could not retrieve PileUpTools");
  }
  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DigitizationAlg::execute() {

  ATH_MSG_DEBUG ("in execute()");

  ToolHandleArray<IPileUpTool>::iterator iPUT(m_puTools.begin());
  ToolHandleArray<IPileUpTool>::iterator ePUT(m_puTools.end());

  StatusCode sc(StatusCode::SUCCESS);
  while (sc.isSuccess() && (iPUT != ePUT)) {
    IPileUpTool& put(**iPUT);
    ATH_MSG_VERBOSE( "Running IPileUpTool " << put.name() );
    sc=put.processAllSubEvents();
    ++iPUT;
  }
  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode DigitizationAlg::finalize() {

  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}
