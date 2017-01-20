/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
// CalibHitToCaloCellAlg.cxx
//     Algorithm to execute CalibHitToCaloCellTool
//  Ioannis Nomidis <ioannis.nomidis@cern.ch>
//  Dec 2016
//*****************************************************************************

#include "CaloCalibHitRec/CalibHitToCaloCellAlg.h"

#include "CaloCalibHitRec/CalibHitToCaloCellTool.h"
#include "CaloEvent/CaloCellContainer.h"


CalibHitToCaloCellAlg::CalibHitToCaloCellAlg(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("CalibHitToCaloCellToolName",m_calibhitToCaloCellTool); 
}

CalibHitToCaloCellAlg::~CalibHitToCaloCellAlg() {}


StatusCode CalibHitToCaloCellAlg::initialize() 
{   
  if(m_calibhitToCaloCellTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_calibhitToCaloCellTool);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool " << m_calibhitToCaloCellTool);

  ATH_MSG_INFO("initialisation completed" );
  return StatusCode::SUCCESS;
}

StatusCode CalibHitToCaloCellAlg::finalize()
{
  ATH_MSG_INFO("finalize() successfully" );
  return StatusCode::SUCCESS;
}

StatusCode CalibHitToCaloCellAlg::execute()
{
  if (!m_calibhitToCaloCellTool->processCalibHitsFromParticle().isSuccess()) {
    ATH_MSG_FATAL("Tool " << m_calibhitToCaloCellTool << " failed.");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG("execute() completed successfully" );
  return StatusCode::SUCCESS;
}

