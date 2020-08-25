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
    //Copy range of data into vector using efficient memmove/memcopy
    statusWords.assign(rawStatus, rawStatus+nStatus);
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("std::exception caught when reading status words: " << ex.what());
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("Unknown exception caught when reading status words");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Successfully read " << statusWords.size() << " status words");
  resultToFill.setStatus(std::move(statusWords)); //statusWords is moved so it now empty

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
  ATH_MSG_DEBUG("Successfully read " << streamTags.size() << " stream tags");
  ATH_CHECK(resultToFill.setStreamTags(streamTags));


  // ---------------------------------------------------------------------------
  // Read the HLT bits
  // ---------------------------------------------------------------------------
  std::vector<uint32_t> hltBitWords;
  // try-catch around eformat calls and raw-pointer operations
  try {
    const uint32_t nHltInfo = rawEvent->nhlt_info();
    const uint32_t* hltInfo = rawEvent->hlt_info(); // owned by rawEvent
    //Copy range of data into vector using efficient memmove/memcopy
    hltBitWords.assign(hltInfo, hltInfo+nHltInfo);
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("std::exception caught when reading HLT bits: " << ex.what());
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("Unknown exception caught when reading HLT bits");
    return StatusCode::FAILURE;
  }
  if (hltBitWords.size() % 3 != 0) {
    ATH_MSG_ERROR("Size of hltBitWords=" << hltBitWords.size() << " must be divisible by three. Expecting {raw, prescaled, rerun} bits.");
    return StatusCode::FAILURE;
  }
  const size_t sizeOfBlock = hltBitWords.size() / 3;
  auto beginPrescaledIt = hltBitWords.begin();
  std::advance(beginPrescaledIt, sizeOfBlock);
  auto beginRerunIt = hltBitWords.begin();
  std::advance(beginRerunIt, 2 * sizeOfBlock);
  resultToFill.setHltPassRawBits( {hltBitWords.begin(), beginPrescaledIt} );
  resultToFill.setHltPrescaledBits( {beginPrescaledIt, beginRerunIt} );
  resultToFill.setHltRerunBits( {beginRerunIt, hltBitWords.end()} );
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
      //Copy range of data into vector using efficient memmove
      data.assign(rodData, rodData+nRodData);
    }
    catch (const std::exception& ex) {
      ATH_MSG_ERROR("std::exception caught when reading HLT result payload: " << ex.what());
      return StatusCode::FAILURE;
    }
    catch (...) {
      ATH_MSG_ERROR("Unknown exception caught when reading HLT result payload");
      return StatusCode::FAILURE;
    }
    size_t datasize = data.size(); //I need to ask size before moving the vector
    ATH_CHECK( resultToFill.addSerialisedDataWithCheck(sid.module_id(), std::move(data)) );
    //Moved "data" so it is now empty vector
    ATH_MSG_DEBUG("Successfully read " << datasize << " words of HLT result payload for module ID "
                  << sid.module_id());
  }
  return StatusCode::SUCCESS;
}
