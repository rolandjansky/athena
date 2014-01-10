/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include "RPC_Digitization/RPC_Digitizer.h"

RPC_Digitizer::RPC_Digitizer(const std::string& name, 
				 ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_digTool("RpcDigitizationTool", this )
{
   declareProperty("RPC_DigitizationTool", m_digTool);
}

RPC_Digitizer::~RPC_Digitizer()  {

}

StatusCode RPC_Digitizer::initialize() {

  if (m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL ( "Could not retrieve RPC Digitization Tool!" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved RPC Digitization Tool." );

  return StatusCode::SUCCESS;
    
}

StatusCode RPC_Digitizer::execute() {

  ATH_MSG_DEBUG ( "in execute()" );

  return m_digTool->digitize();
}

StatusCode RPC_Digitizer::finalize() {
    
   ATH_MSG_DEBUG ( "finalize." );

   return StatusCode::SUCCESS;
}


