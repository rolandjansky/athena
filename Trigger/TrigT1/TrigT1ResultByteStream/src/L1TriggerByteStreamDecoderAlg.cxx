/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TriggerByteStreamDecoderAlg.h"

// =============================================================================
// Standard constructor
// =============================================================================
L1TriggerByteStreamDecoderAlg::L1TriggerByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc)
: AthReentrantAlgorithm(name, svcLoc) {}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode L1TriggerByteStreamDecoderAlg::initialize() {
  ATH_MSG_DEBUG("Initialising " << name());
  ATH_CHECK(m_decoderTools.retrieve());
  ATH_CHECK(m_robDataProviderSvc.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::finalize
// =============================================================================
StatusCode L1TriggerByteStreamDecoderAlg::finalize() {
  ATH_MSG_DEBUG("Finalising " << name());
  ATH_CHECK(m_robDataProviderSvc.release());
  ATH_CHECK(m_decoderTools.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode L1TriggerByteStreamDecoderAlg::execute(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // Retrieve the BS data and call the decoder tools
  IROBDataProviderSvc::VROBFRAG vrobf;
  for (const auto& decoderTool : m_decoderTools) {
    vrobf.clear();
    m_robDataProviderSvc->getROBData(eventContext, decoderTool->robIds(), vrobf, name());
    ATH_CHECK(decoderTool->convertFromBS(vrobf, eventContext));
  }

  return StatusCode::SUCCESS;
}
