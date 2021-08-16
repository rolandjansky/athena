/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "HLTResultMTByteStreamDecoderTool.h"

// TDAQ includes
#include "eformat/StreamTag.h"

using RODMinorVersion = HLT::HLTResultMT::RODMinorVersion;

namespace{
  // Convert RODMinorVersion to string
  std::string printRodMinorVersion(const RODMinorVersion v) {
    return std::to_string(v.first) + "." + std::to_string(v.second);
  }
}

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
RODMinorVersion HLTResultMTByteStreamDecoderTool::getHltRodMinorVersion(
  const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf) const {

  if (vrobf.empty()) {
    ATH_MSG_ERROR("Empty ROBFragment vector passed to getHltRodMinorVersion, returning invalid version");
    return {0xff, 0xff};
  }

  uint16_t version{0xffff};
  bool first{true};
  for (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robf : vrobf) {
    uint32_t thisRodVersion = robf->rod_version();
    uint16_t thisRodMinorVersion = (thisRodVersion & 0xffffu);
    if (thisRodMinorVersion == 0xffffu) {
      ATH_MSG_WARNING("Invalid HLT ROD minor version 0xffff found in ROBFragment 0x"
                      << MSG::hex << robf->rob_source_id() << MSG::dec);
    }
    if (first) {
      version = thisRodMinorVersion;
    }
    else if (thisRodMinorVersion != version) {
      ATH_MSG_ERROR("Inconsistent HLT ROD minor versions in different ROBFragments, 0x"
                    << MSG::hex << version << " and " << thisRodMinorVersion << MSG::dec
                    << ", getHltRodMinorVersion returning invalid version");
      return {0xff, 0xff};
    }
    first = false;
  }
  // Split the version 0xabcd into {0xab, 0xcd}
  return {((version & 0xff00u) >> 8u), (version & 0x00ffu)};
}

// =============================================================================
StatusCode HLTResultMTByteStreamDecoderTool::decodeHeader(const RawEvent* rawEvent, HLT::HLTResultMT& resultToFill) const {

  if (!rawEvent) {
    ATH_MSG_ERROR("Decoding requested with nullptr RawEvent");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Decoding HLTResultMT from ByteStream event " << rawEvent->global_id());

  const RODMinorVersion hltRodMinorVersion = resultToFill.getVersion();
  ATH_CHECK(checkRodMinorVersion(hltRodMinorVersion));

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

  // Version 1.0 includes rerun bits, removed since version 1.1
  const size_t numBitSets = (hltRodMinorVersion==RODMinorVersion{1,0}) ? 3 : 2;

  if (hltBitWords.size() % numBitSets != 0) {
    ATH_MSG_ERROR("Size of hltBitWords=" << hltBitWords.size() << " must be divisible by " << numBitSets
                  << ". Expecting {raw, prescaled" << (numBitSets==3 ? ", rerun" : "") << "} bits.");
    return StatusCode::FAILURE;
  }
  const size_t sizeOfBlock = hltBitWords.size() / numBitSets;
  auto beginPrescaledIt = hltBitWords.cbegin() + sizeOfBlock;
  auto endIt = (numBitSets==2) ? hltBitWords.cend() : hltBitWords.cbegin() + 2*sizeOfBlock;
  resultToFill.setHltBits( {hltBitWords.cbegin(), beginPrescaledIt},
                           {beginPrescaledIt, endIt} );
  ATH_MSG_DEBUG("Successfully read " << std::distance(hltBitWords.cbegin(), endIt) << " HLT bit words");
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode HLTResultMTByteStreamDecoderTool::decodePayload(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                                                           HLT::HLTResultMT& resultToFill) const {
  const RODMinorVersion hltRodMinorVersion = resultToFill.getVersion();
  ATH_CHECK(checkRodMinorVersion(hltRodMinorVersion));

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

// =============================================================================
StatusCode HLTResultMTByteStreamDecoderTool::checkRodMinorVersion(const RODMinorVersion version) const {
  if (version == RODMinorVersion{0xff, 0xff}) {
    ATH_MSG_ERROR("Invalid HLT ROD minor version {0xff, 0xff}");
    return StatusCode::FAILURE;
  }
  if (version.first < 1) {
    ATH_MSG_ERROR("HLT ROD minor version " << printRodMinorVersion(version) << " is lower than 1.0. "
                  << "This tool is for decoding versions 1.0 or later (i.e. HLT ByteStream from Run 3 or later)");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("HLT ROD minor version is " << printRodMinorVersion(version));
  return StatusCode::SUCCESS;
}
