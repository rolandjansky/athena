/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_Digitization/TGCDigitizer.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

TGCDigitizer::TGCDigitizer(const std::string& name, 
			   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_digTool("TgcDigitizationTool", this)
{
  declareProperty("TGC_DigitizationTool", m_digTool);
}

TGCDigitizer::~TGCDigitizer() {
}

StatusCode TGCDigitizer::initialize() {
  if(m_digTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve TGC Digitization Tool!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved TGC Digitization Tool (" << m_digTool->name() << ").");
  
  return StatusCode::SUCCESS;
}

StatusCode TGCDigitizer::execute() {
  ATH_MSG_DEBUG("in execute()");
  return m_digTool->digitize();
}

StatusCode TGCDigitizer::finalize() {
  ATH_MSG_DEBUG("finalize.");
  return StatusCode::SUCCESS;
}
