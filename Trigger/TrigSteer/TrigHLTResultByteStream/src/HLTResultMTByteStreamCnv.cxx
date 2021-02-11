/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "TrigHLTResultByteStream/HLTResultMTByteStreamCnv.h"
#include "TrigSteeringEvent/HLTResultMT.h"

// Athena includes
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/StorableConversions.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"

// Gaudi includes
#include "GaudiKernel/IRegistry.h"

// TDAQ includes
#include "eformat/Issue.h"
#include "eformat/SourceIdentifier.h"

// Local constants
namespace {
  /**
   * ROBFragment module ID of the HLT result which contains all Trigger EDM collections. Can be considered to be made
   * a property set from the EDM configuration to avoid multiple definitions, but the value should never change from 0.
   */
  constexpr uint16_t fullResultModuleId = 0;
  /**
   * HLT ROBFragment ROD minor version.
   * Changed from 0.0 to 1.0 in September 2019 to differentiate Run-3 HLT ByteStream format from earlier formats.
   */
  constexpr uint16_t hltRodMinorVersion = 0x0100;
}

// Local helper methods
namespace {
  /// Check if a stream tag has the given type
  bool isStreamTagType(const eformat::helper::StreamTag& st, const eformat::TagType type) {
    return eformat::helper::string_to_tagtype(st.type) == type;
  }
  /// Check if a stream tag has the debug type
  bool isDebugStreamTag(const eformat::helper::StreamTag& st) {
    return isStreamTagType(st, eformat::TagType::DEBUG_TAG);
  }
  /// Check if a stream tag has the unknown type
  bool isUnknownStreamTag(const eformat::helper::StreamTag& st) {
    return isStreamTagType(st, eformat::TagType::UNKNOWN_TAG);
  }
}

// =============================================================================
// Standard constructor
// =============================================================================
HLT::HLTResultMTByteStreamCnv::HLTResultMTByteStreamCnv(ISvcLocator* svcLoc) :
  Converter(storageType(), classID(), svcLoc),
  AthMessaging(msgSvc(), "HLTResultMTByteStreamCnv"),
  m_ByteStreamEventAccess("ByteStreamCnvSvc", "HLTResultMTByteStreamCnv") {}

// =============================================================================
// Standard destructor
// =============================================================================
HLT::HLTResultMTByteStreamCnv::~HLTResultMTByteStreamCnv() {}

// =============================================================================
// Implementation of Converter::initialize
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::initialize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_CHECK(m_ByteStreamEventAccess.retrieve());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Converter::finalize
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::finalize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  if (m_ByteStreamEventAccess.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_ByteStreamEventAccess.typeAndName());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Converter::createObj
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::createObj(IOpaqueAddress* /*pAddr*/, DataObject*& /*pObj*/) {
  ATH_MSG_ERROR("Using BS converter to decode HLTResultMT is not supported!"
                << " Use HLTResultMTByteStreamDecoderAlg instead");
  return StatusCode::FAILURE;
}

// =============================================================================
// Implementation of Converter::createRep
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  // Clear the cache which is required to remain valid between being filled here
  // and sending out the correspoding RawEventWrite in (Trig)ByteStreamCnvSvc::commitOutput
  m_cache.clear();

  // Cast the DataObject to HLTResultMT
  HLT::HLTResultMT* hltResult = nullptr;
  bool castSuccessful = SG::fromStorable(pObj, hltResult);
  if (!castSuccessful || !hltResult) {
    ATH_MSG_ERROR("Failed to convert DataObject to HLTResultMT");
    return StatusCode::FAILURE;
  }

  // Obtain the RawEventWrite (aka eformat::write::FullEventFragment) pointer
  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();
  if (!re) {
    ATH_MSG_ERROR("Failed to obtain a pointer to RawEventWrite");
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Obtained RawEventWrite pointer = " << re);

  // Fill the status words (error code)
  re->status(hltResult->getStatus().size(), hltResult->getStatus().data());

  // Read the stream tags to check for debug stream tag and decide which HLT ROBFragments to write out
  std::set<eformat::helper::SourceIdentifier> resultIdsToWrite;
  bool debugEvent=false;
  std::string unknownTypeStreams;
  for (const eformat::helper::StreamTag& st : hltResult->getStreamTags()) {
    // Flag events with unknown stream type
    if (isUnknownStreamTag(st)) {
      unknownTypeStreams += st.type + "_" + st.name + " ";
    }
    // Flag debug stream events
    if (isDebugStreamTag(st)) debugEvent=true;
    // In case of full event building, add the full result ID
    if (st.robs.empty() && st.dets.empty()) {
      eformat::helper::SourceIdentifier sid(eformat::SubDetector::TDAQ_HLT, fullResultModuleId);
      resultIdsToWrite.insert(sid);
    }
    // In case of partial event building, add the results explicitly requested in the stream tag
    for (const uint32_t robid : st.robs) {
      eformat::helper::SourceIdentifier sid(robid);
      if (sid.subdetector_id() == eformat::SubDetector::TDAQ_HLT)
        resultIdsToWrite.insert(sid);
    }
  }
  // Fail if the event is not already failed (debug stream) and unknown stream type was found
  if (!debugEvent && !unknownTypeStreams.empty()) {
    ATH_MSG_ERROR("Found stream tag(s) with unknown type: " << unknownTypeStreams);
    return StatusCode::FAILURE;
  }

  // If the event goes to the debug stream, remove all non-debug stream tags
  // and force full event building in all debug streams
  if (debugEvent) {
    std::vector<eformat::helper::StreamTag>& writableStreamTags = hltResult->getStreamTagsNonConst();
    writableStreamTags.erase(
      std::remove_if(writableStreamTags.begin(),writableStreamTags.end(),std::not_fn(isDebugStreamTag)),
      writableStreamTags.end()
    );
    for (eformat::helper::StreamTag& st : writableStreamTags) {
      st.robs.clear();
      st.dets.clear();
    }
  }

  // Fill the stream tags
  uint32_t nStreamTagWords = eformat::helper::size_word(hltResult->getStreamTags());
  m_cache.streamTagData = std::make_unique<uint32_t[]>(nStreamTagWords);
  try {
    // encode can throw exceptions if the encoding fails
    eformat::helper::encode(hltResult->getStreamTags(),nStreamTagWords,m_cache.streamTagData.get());
  }
  catch (const std::exception& e) {
    ATH_MSG_ERROR("StreamTag encoding failed, caught an unexpected std::exception " << e.what());
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("StreamTag encoding failed, caught an unexpected exception");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Encoded the stream tags successfully");
  re->stream_tag(nStreamTagWords, m_cache.streamTagData.get());

  // Fill the HLT bits
  const std::vector<uint32_t>& hltBits = hltResult->getHltBitsAsWords();
  re->hlt_info(hltBits.size(), hltBits.data());

  // Loop over the module IDs and fill the ROBFragments
  ATH_MSG_DEBUG("Iterating over " << resultIdsToWrite.size() << " HLT result IDs to assemble output data");
  const std::unordered_map<uint16_t, std::vector<uint32_t>>& serialisedData = hltResult->getSerialisedData();
  for (const eformat::helper::SourceIdentifier& resultId : resultIdsToWrite) {
    // Find the serialised data for this module ID
    const auto it = serialisedData.find(resultId.module_id());
    if (it==serialisedData.end()) {
      if (debugEvent) {
        ATH_MSG_DEBUG("HLT result with ID 0x" << MSG::hex << resultId.code() << MSG::dec
                    << " requested by a debug stream tag, but missing in the serialised data - skipping this result");
        continue;
      }
      ATH_MSG_ERROR("HLT result with ID 0x" << MSG::hex << resultId.code() << MSG::dec
                    << " requested by a stream tag, but missing in the serialised data");
      return StatusCode::FAILURE;
    }
    const std::vector<uint32_t>& data = it->second;

    // Create an HLT ROBFragment and append it to the full event
    m_cache.robFragments.push_back(std::make_unique<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment>(
      resultId.code(),
      re->run_no(),
      re->lvl1_id(),
      re->bc_id(),
      re->lvl1_trigger_type(),
      0, // detev_type not used by HLT
      data.size(),
      data.data(),
      eformat::STATUS_BACK
    ));
    m_cache.robFragments.back()->rod_minor_version(hltRodMinorVersion);
    re->append(m_cache.robFragments.back().get());
    ATH_MSG_DEBUG("Appended data for HLT result ID 0x" << MSG::hex << resultId.code() << MSG::dec << " with "
                  << data.size() << " words of serialised payload to the output full event");
  }

  // Create a ByteStreamAddress for HLTResultMT
  ByteStreamAddress* bsAddr = new ByteStreamAddress(classID(), pObj->registry()->name(), "");
  pAddr = static_cast<IOpaqueAddress*>(bsAddr);

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// CLID / storageType
// =============================================================================
const CLID& HLT::HLTResultMTByteStreamCnv::classID() {
  return ClassID_traits<HLT::HLTResultMT>::ID();
}

long HLT::HLTResultMTByteStreamCnv::storageType()
{
  return ByteStreamAddress::storageType();
}
