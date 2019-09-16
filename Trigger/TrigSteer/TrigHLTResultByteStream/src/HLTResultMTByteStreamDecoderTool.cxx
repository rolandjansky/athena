/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "HLTResultMTByteStreamDecoderTool.h"

// TDAQ includes
#include "eformat/StreamTag.h"

// =============================================================================
// Standard constructor
// =============================================================================
HLTResultMTByteStreamDecoderTool::HLTResultMTByteStreamDecoderTool(const std::string& type,
                                                                   const std::string& name,
                                                                   const IInterface* parent)
: AthAlgTool(type,name,parent) {}

// =============================================================================
// Implementation of AthAlgTool::initialize
// =============================================================================
StatusCode HLTResultMTByteStreamDecoderTool::initialize() {
  ATH_MSG_DEBUG("Initialising " << name());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthAlgTool::finalize
// =============================================================================
StatusCode HLTResultMTByteStreamDecoderTool::finalize() {
  ATH_MSG_DEBUG("Finalising " << name());
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode HLTResultMTByteStreamDecoderTool::decodeHeader(const RawEvent* rawEvent, HLT::HLTResultMT& resultToFill) const {

  if (!rawEvent) {
    ATH_MSG_ERROR("Decoding requested with nullptr RawEvent");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Decoding HLTResultMT from ByteStream event " << rawEvent->global_id());

  // ---------------------------------------------------------------------------
  // Read the status words (error codes from online event processing)
  // ---------------------------------------------------------------------------
  std::vector<uint32_t> statusWords;
  // try-catch around eformat calls and raw-pointer operations
  try {
    const uint32_t nStatus = rawEvent->nstatus();
    const uint32_t* rawStatus = rawEvent->status(); // owned by rawEvent
    for (uint32_t i=0; i<nStatus; ++i)
      statusWords.push_back(rawStatus[i]);
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("std::exception caught when reading status words: " << ex.what());
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("Unknown exception caught when reading status words");
    return StatusCode::FAILURE;
  }
  resultToFill.setStatus(statusWords);
  ATH_MSG_DEBUG("Successfully read " << statusWords.size() << " status words");

  // ---------------------------------------------------------------------------
  // Read the stream tags
  // ---------------------------------------------------------------------------
  std::vector<eformat::helper::StreamTag> streamTags;
  // try-catch around eformat calls and raw-pointer operations
  try {
    eformat::helper::decode(rawEvent->nstream_tag(),rawEvent->stream_tag(),streamTags);
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("std::exception caught when reading stream tags: " << ex.what());
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("Unknown exception caught when reading stream tags");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(resultToFill.setStreamTags(streamTags));
  ATH_MSG_DEBUG("Successfully read " << streamTags.size() << " stream tags");

  // ---------------------------------------------------------------------------
  // Read the HLT bits
  // ---------------------------------------------------------------------------
  std::vector<uint32_t> hltBitWords;
  // try-catch around eformat calls and raw-pointer operations
  try {
    const uint32_t nHltInfo = rawEvent->nhlt_info();
    const uint32_t* hltInfo = rawEvent->hlt_info(); // owned by rawEvent
    for (uint32_t i=0; i<nHltInfo; ++i)
      hltBitWords.push_back(hltInfo[i]);
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("std::exception caught when reading HLT bits: " << ex.what());
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("Unknown exception caught when reading HLT bits");
    return StatusCode::FAILURE;
  }
  resultToFill.setHltBits( {hltBitWords.begin(), hltBitWords.end()} );
  ATH_MSG_DEBUG("Successfully read " << hltBitWords.size() << " HLT bit words");

  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode HLTResultMTByteStreamDecoderTool::decodePayload(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                                                           HLT::HLTResultMT& resultToFill) const {
  for (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robf : vrobf) {
    eformat::helper::SourceIdentifier sid(robf->rob_source_id());
    ATH_MSG_DEBUG("Reading ROBFragment " << sid.human());
    std::vector<uint32_t> data;
    // try-catch around eformat calls and raw-pointer operations
    try {
      const uint32_t nRodData = robf->rod_ndata();
      const uint32_t* rodData = robf->rod_data(); // owned by robf
      for (uint32_t i=0; i<nRodData; ++i)
        data.push_back(rodData[i]);
    }
    catch (const std::exception& ex) {
      ATH_MSG_ERROR("std::exception caught when reading HLT result payload: " << ex.what());
      return StatusCode::FAILURE;
    }
    catch (...) {
      ATH_MSG_ERROR("Unknown exception caught when reading HLT result payload");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( resultToFill.addSerialisedDataWithCheck(sid.module_id(), data) );
    ATH_MSG_DEBUG("Successfully read " << data.size() << " words of HLT result payload for module ID "
                  << sid.module_id());
  }
  return StatusCode::SUCCESS;
}
