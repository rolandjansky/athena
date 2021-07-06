/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TriggerByteStreamDecoderAlg.h"
#include <algorithm>

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

  for (const auto& decoderTool : m_decoderTools) {
    const std::vector<uint32_t>& ids = decoderTool->robIds();
    m_robIds.insert(m_robIds.end(), ids.begin(), ids.end());
  }
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

  // Retrieve the BS data for all tools in one request to parallelise DCM->ROS network requests
  IROBDataProviderSvc::VROBFRAG vrobf;
  m_robDataProviderSvc->getROBData(eventContext, m_robIds, vrobf, name());

  // Filter ROB list for each tool and call the conversion
  for (const auto& decoderTool : m_decoderTools) {
    IROBDataProviderSvc::VROBFRAG vrobfForTool;
    ATH_CHECK(filterRobs(vrobf, vrobfForTool, decoderTool->robIds(), decoderTool->name()));
    ATH_CHECK(decoderTool->convertFromBS(vrobfForTool, eventContext));
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode L1TriggerByteStreamDecoderAlg::filterRobs(const IROBDataProviderSvc::VROBFRAG& in,
                                                     IROBDataProviderSvc::VROBFRAG& out,
                                                     const std::vector<uint32_t>& ids,
                                                     std::string_view toolName) const {
  std::copy_if(in.cbegin(), in.cend(), std::back_inserter(out),
               [&ids](const IROBDataProviderSvc::ROBF* rob){
                 return (std::find(ids.cbegin(),ids.cend(),rob->source_id()) != ids.cend());
               });
  // Check if all ROBs were found and report errors if not
  if (ids.size() != out.size()) {
    for (const uint32_t id : ids) {
      auto eqRobId = [&id](const IROBDataProviderSvc::ROBF* rob){return (rob->source_id() == id);};
      if (std::find_if(out.cbegin(),out.cend(),eqRobId) == out.cend()) {
        ATH_MSG_ERROR("Missing ROBFragment with ID 0x" << MSG::hex << id << MSG::dec << " requested by " << toolName);
      }
    }
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found all " << ids.size() << " ROBFragments requested by " << toolName);
  return StatusCode::SUCCESS;
}
