/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_Digitization/ALFA_DigiAlg.h"
#include "PileUpTools/IPileUpTool.h"

ALFA_DigiAlg::ALFA_DigiAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_digiTool("ALFA_PileUpTool", this )
{
  declareProperty("DigitizationTool", m_digiTool, "AthAlgTool which performs ALFA digitization");
}

StatusCode ALFA_DigiAlg::initialize() 
{
 // intitialize store gate active store
  if (m_digiTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve ALFA_PileUpTool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved ALFA_PileUpTool." );
  return StatusCode::SUCCESS;
}

StatusCode ALFA_DigiAlg::execute() 
{
  ATH_MSG_DEBUG ( "ALFA_DigiAlg::execute" );
  
  return m_digiTool->processAllSubEvents();
}

StatusCode ALFA_DigiAlg::finalize() 
{
  return StatusCode::SUCCESS; 
}
