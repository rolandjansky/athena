/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_SimuDigitization/ZDC_DigiTop.h"
#include "PileUpTools/IPileUpTool.h"

ZDC_DigiTop::ZDC_DigiTop(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_digTool("ZDC_PileUpTool", this )
{
  declareProperty("DigitizationTool", m_digTool, "AthAlgTool which performs the ZDC digitization");
}

StatusCode ZDC_DigiTop::initialize() 
{
 // intitialize store gate active store
  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve ZDC_PileUpTool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved ZDC_PileUpTool." );
  return StatusCode::SUCCESS;
}

StatusCode ZDC_DigiTop::execute() 
{
  ATH_MSG_DEBUG ( "ZDC_DigiTop::execute" );
  
  return m_digTool->processAllSubEvents();
}

StatusCode ZDC_DigiTop::finalize() 
{
  return StatusCode::SUCCESS; 
}











