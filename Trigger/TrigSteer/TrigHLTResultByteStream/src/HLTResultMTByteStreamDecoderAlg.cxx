/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTResultMTByteStreamDecoderAlg.h"

// =============================================================================
// Standard constructor
// =============================================================================
HLTResultMTByteStreamDecoderAlg::HLTResultMTByteStreamDecoderAlg(const std::string& name, ISvcLocator* svcLoc)
: AthReentrantAlgorithm(name, svcLoc),
  m_robDataProviderSvc("ROBDataProviderSvc", name) {}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode HLTResultMTByteStreamDecoderAlg::initialize() {
  ATH_MSG_INFO("Initialising " << name());
  ATH_CHECK(m_hltResultWHKey.initialize());
  ATH_CHECK(m_robDataProviderSvc.retrieve());

  // Convert module IDs to HLT result ROB IDs
  for (const uint16_t moduleId : m_moduleIdsToDecode) {
    eformat::helper::SourceIdentifier sid(eformat::TDAQ_HLT, moduleId);
    m_robIdsToDecode.push_back(sid.code());
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::finalize
// =============================================================================
StatusCode HLTResultMTByteStreamDecoderAlg::finalize() {
  ATH_MSG_INFO("Finalising " << name());
  ATH_CHECK(m_robDataProviderSvc.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute_r
// =============================================================================
StatusCode HLTResultMTByteStreamDecoderAlg::execute_r(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // Create and record the HLTResultMT object
  auto hltResult = SG::makeHandle(m_hltResultWHKey,eventContext);
  ATH_CHECK( hltResult.record(std::make_unique<HLT::HLTResultMT>()) );
  ATH_MSG_DEBUG("Recorded HLTResultMT with key " << m_hltResultWHKey.key());

  // Retrieve the RawEvent pointer
  const RawEvent* re = m_robDataProviderSvc->getEvent(eventContext); // Owned by ROBDataProvider or InputSvc
  if (!re) {
    ATH_MSG_ERROR("Failed to retrieve the RawEvent pointer from ROBDataProvider");
    return StatusCode::FAILURE;
  }

  // Fill the result object from ByteStream event header
  ATH_CHECK(m_decoderTool->decodeHeader(re, *hltResult));

  // Read the HLT result payload
  IROBDataProviderSvc::VROBFRAG vrobf; // vector of ROBFragments to be filled
  m_robDataProviderSvc->getROBData(m_robIdsToDecode, vrobf, name());
  ATH_CHECK(m_decoderTool->decodePayload(vrobf, *hltResult));

  // Print the result
  ATH_MSG_DEBUG(*hltResult);

  return StatusCode::SUCCESS;
}
