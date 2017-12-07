/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_LinkMaskingTestAlg.cxx
 *
 * @brief 
 * 
 *
 * @author gwilliam@mail.cern.ch
 **/

#include "SCT_LinkMaskingTestAlg.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

SCT_LinkMaskingTestAlg::SCT_LinkMaskingTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_linkMaskingSvc("SCT_LinkMaskingSvc", name) {
  declareProperty("LinkMaskingSvc", m_linkMaskingSvc);
}

SCT_LinkMaskingTestAlg::~SCT_LinkMaskingTestAlg() { 
  ATH_MSG_INFO("Calling destructor");
}

//Initialize
StatusCode SCT_LinkMaskingTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  
  // Retrieve link masking service
  if (m_linkMaskingSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve the link masking service");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_LinkMaskingTestAlg::execute() {

  ATH_MSG_INFO("Wafer 167772160 is " << (m_linkMaskingSvc->isGood(Identifier{167772160}) ? "not masked" : "masked"));
  ATH_MSG_INFO("Wafer 167773184 is " << (m_linkMaskingSvc->isGood(Identifier{167773184}) ? "not masked" : "masked"));
  ATH_MSG_INFO("Wafer 167786496 is " << (m_linkMaskingSvc->isGood(Identifier{167786496}) ? "not masked" : "masked"));
  ATH_MSG_INFO("Wafer 167787520 is " << (m_linkMaskingSvc->isGood(Identifier{167787520}) ? "not masked" : "masked"));

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode SCT_LinkMaskingTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
