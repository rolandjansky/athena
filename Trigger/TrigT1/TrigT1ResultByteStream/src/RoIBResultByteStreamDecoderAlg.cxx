/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "RoIBResultByteStreamDecoderAlg.h"

// =============================================================================
// Standard constructor
// =============================================================================
RoIBResultByteStreamDecoderAlg::RoIBResultByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc)
: AthReentrantAlgorithm(name, svcLoc),
  m_robDataProviderSvc("ROBDataProviderSvc", name) {}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode RoIBResultByteStreamDecoderAlg::initialize() {
  ATH_MSG_DEBUG("Initialising " << name());
  ATH_CHECK(m_roibResultWHKey.initialize());
  ATH_CHECK(m_robDataProviderSvc.retrieve());

  // Retrieve the list of ROB IDs to decode
  m_robIdsToDecode = m_decoderTool->configuredROBIds();
  ATH_MSG_DEBUG("List of ROB IDs to decode:");
  for (const uint32_t robid : m_robIdsToDecode) {
    ATH_MSG_DEBUG("   0x" << MSG::hex << robid << MSG::dec);
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::finalize
// =============================================================================
StatusCode RoIBResultByteStreamDecoderAlg::finalize() {
  ATH_MSG_DEBUG("Finalising " << name());
  ATH_CHECK(m_robDataProviderSvc.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode RoIBResultByteStreamDecoderAlg::execute(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // Create and record the RoIBResult object
  auto roibResult = SG::makeHandle(m_roibResultWHKey,eventContext);
  ATH_CHECK( roibResult.record(std::make_unique<ROIB::RoIBResult>()) );
  ATH_MSG_DEBUG("Recorded RoIBResult with key " << m_roibResultWHKey.key());

  // Retrieve the BS data and fill the RoIBResult
  IROBDataProviderSvc::VROBFRAG vrobf; // vector of ROBFragments to be filled
  m_robDataProviderSvc->getROBData(eventContext, m_robIdsToDecode, vrobf, name());
  ATH_CHECK(m_decoderTool->convert(vrobf, *roibResult));

  return StatusCode::SUCCESS;
}
