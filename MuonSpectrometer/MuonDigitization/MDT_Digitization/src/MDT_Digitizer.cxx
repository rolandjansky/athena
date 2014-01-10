/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include "MDT_Digitization/MDT_Digitizer.h"

MDT_Digitizer::MDT_Digitizer(const std::string& name, 
				 ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_digTool("MdtDigitizationTool", this )
{
   declareProperty("MDT_DigitizationTool", m_digTool);
}

MDT_Digitizer::~MDT_Digitizer()  {

}

StatusCode MDT_Digitizer::initialize() {

  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve MDT Digitization Tool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved MDT Digitization Tool." );

  return StatusCode::SUCCESS;
    
}

StatusCode MDT_Digitizer::execute() {

  ATH_MSG_DEBUG ( "in execute()" );

  return m_digTool->digitize();
}

 StatusCode MDT_Digitizer::finalize() {
    
   ATH_MSG_DEBUG ( "finalize." );

   return StatusCode::SUCCESS;
}


