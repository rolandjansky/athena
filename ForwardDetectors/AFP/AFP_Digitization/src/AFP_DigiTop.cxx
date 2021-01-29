/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Digitization/AFP_DigiTop.h"
//#include "PileUpTools/IPileUpTool.h" //Temporary for back-compatibility with 17.3.X.Y
#include "AFP_Digitization/AFP_PileUpTool.h" //Temporary for back-compatibility with 17.3.X.Y
AFP_DigiTop::AFP_DigiTop(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_digTool("AFP_PileUpTool", this )
{
  declareProperty("DigitizationTool", m_digTool, "AthAlgTool which performs the AFP digitization");
}

StatusCode AFP_DigiTop::initialize() 
{
 // intitialize store gate active store

  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve AFP_PileUpTool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved AFP_PileUpTool." );
  return StatusCode::SUCCESS;
}

StatusCode AFP_DigiTop::execute() 
{
  ATH_MSG_DEBUG ( "AFP_DigiTop::execute" );

  CHECK ( m_digTool->processAllSubEvents() );

  return StatusCode::SUCCESS;
}

StatusCode AFP_DigiTop::finalize() 
{
  return StatusCode::SUCCESS; 
}


