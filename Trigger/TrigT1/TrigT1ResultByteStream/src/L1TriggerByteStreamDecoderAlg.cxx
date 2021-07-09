/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TriggerByteStreamDecoderAlg.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"
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
  ATH_CHECK(m_bsMetaDataContRHKey.initialize(SG::AllowEmpty));

  for (const auto& decoderTool : m_decoderTools) {
    const std::vector<uint32_t>& ids = decoderTool->robIds();
    m_robIds.insert(m_robIds.end(), ids.begin(), ids.end());
  }

  // Parse properties
  m_maybeMissingRobs.insert(m_maybeMissingRobsProp.value().begin(), m_maybeMissingRobsProp.value().end());
  if (m_robStatusCheckLevel.value() == "None") {m_robStatusCheck=ROBStatusCheck::None;}
  else if (m_robStatusCheckLevel.value() == "Warning") {m_robStatusCheck=ROBStatusCheck::Warning;}
  else if (m_robStatusCheckLevel.value() == "Error") {m_robStatusCheck=ROBStatusCheck::Error;}
  else if (m_robStatusCheckLevel.value() == "Fatal") {m_robStatusCheck=ROBStatusCheck::Fatal;}
  else {
    ATH_MSG_ERROR(m_robStatusCheckLevel.name() << " is set to \"" << m_robStatusCheckLevel.value()
                  << "\" but needs to be one of [\"None\", \"Warning\", \"Error\", \"Fatal\"]");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::start
// =============================================================================
StatusCode L1TriggerByteStreamDecoderAlg::start() {
  // Skip detector mask checks if ByteStreamMetaData not available
  if (m_bsMetaDataContRHKey.key().empty()) {
    return StatusCode::SUCCESS;
  }

  // Get the detector mask
  auto bsmdc = SG::makeHandle(m_bsMetaDataContRHKey);
  ATH_CHECK(bsmdc.isValid());
  ATH_CHECK(!bsmdc->empty());
  const ByteStreamMetadata* metadata = bsmdc->front();
  const eformat::helper::DetectorMask detectorMask{metadata->getDetectorMask(), metadata->getDetectorMask2()};

  // Check if the requested ROBs should be expected to be present or may be missing
  for (const uint32_t robId : m_robIds) {
    const eformat::helper::SourceIdentifier sid{robId};
    if (!detectorMask.is_set(sid.subdetector_id())) {
      if (m_maybeMissingRobs.insert(robId).second) {
        ATH_MSG_WARNING("ROB ID 0x" << MSG::hex << robId << MSG::dec << " was requested for decoding, "
                        << "but the SubDetector " << sid.human_detector() << " is disabled in "
                        << "the detector mask. Will not require this ROB ID to be present in every event.");
      }
    }
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
    ATH_CHECK(checkRobs(vrobfForTool, decoderTool->name()));
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
    bool fail{false};
    for (const uint32_t id : ids) {
      auto eqRobId = [&id](const IROBDataProviderSvc::ROBF* rob){return (rob->source_id() == id);};
      if (std::find_if(out.cbegin(),out.cend(),eqRobId) == out.cend()) {
        if (std::find(m_maybeMissingRobs.cbegin(),m_maybeMissingRobs.cend(),id) != m_maybeMissingRobs.cend()) {
          ATH_MSG_DEBUG("Missing ROBFragment with ID 0x" << MSG::hex << id << MSG::dec << " requested by " << toolName
                        << " but it is allowed to be missing due to the detector mask or the "
                        << m_maybeMissingRobsProp.name() << " property");
        } else {
          ATH_MSG_ERROR("Missing ROBFragment with ID 0x" << MSG::hex << id << MSG::dec << " requested by " << toolName);
          fail = true;
        }
      }
    }
    if (fail) {
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("Found " << out.size() << " out of " << ids.size() << " ROBFragments requested by " << toolName);
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode L1TriggerByteStreamDecoderAlg::checkRobs(const IROBDataProviderSvc::VROBFRAG& robs,
                                                    std::string_view toolName) const {
  StatusCode sc{StatusCode::SUCCESS};
  if (m_robStatusCheck == ROBStatusCheck::None) {
    return sc;
  }
  for (const IROBDataProviderSvc::ROBF* rob : robs) {
    if (rob->nstatus()>0 && rob->status()[0]!=0) {
      std::ostringstream ss;
      ss << "ROBFragment with ID 0x" << std::hex << rob->rob_source_id() << std::dec
         << " requested by " << toolName << " has non-zero status " << rob->status()[0];
      if (m_robStatusCheck==ROBStatusCheck::Warning) {ATH_MSG_WARNING(ss.str());}
      else if (m_robStatusCheck==ROBStatusCheck::Error) {ATH_MSG_ERROR(ss.str());}
      else if (m_robStatusCheck==ROBStatusCheck::Fatal) {
        ATH_MSG_ERROR(ss.str());
        sc = StatusCode::FAILURE;
      }
    }
  }
  return sc;
}
