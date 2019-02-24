/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "TrigHLTResultByteStream/HLTResultMTByteStreamCnv.h"
#include "TrigSteeringEvent/HLTResultMT.h"

// Athena includes
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/StorableConversions.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"

// Gaudi includes
#include "GaudiKernel/IRegistry.h"

// TDAQ includes
#include "eformat/Issue.h"
#include "eformat/SourceIdentifier.h"

// Local definitions
namespace {
  /**
   * ROBFragment module ID of the HLT result which contains all Trigger EDM collections. Can be considered to be made
   * a property set from the EDM configuration to avoid multiple definitions, but the value should never change from 0.
   */
  constexpr uint16_t fullResultModuleId = 0;
}

// =============================================================================
// Standard constructor
// =============================================================================
HLT::HLTResultMTByteStreamCnv::HLTResultMTByteStreamCnv(ISvcLocator* svcLoc) :
  Converter(storageType(), classID(), svcLoc),
  AthMessaging(msgSvc(), "HLTResultMTByteStreamCnv"),
  m_ByteStreamEventAccess("ByteStreamCnvSvc", "HLTResultMTByteStreamCnv") {
    m_fullEventAssembler.idMap().setDetId(eformat::TDAQ_HLT);
  }

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
  ATH_REPORT_ERROR(StatusCode::FAILURE) << "Using BS converter to decode HLTResultMT is not supported!"
                                        << " Use HLTResultMTByteStreamDecoderAlg instead";
  return StatusCode::FAILURE;
}

// =============================================================================
// Implementation of Converter::createRep
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

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

  // Clear the stream tag buffer - we need to keep the serialised StreamTag data cached because
  // its lifetime has to be at least as long as the lifetime of RawEventWrite which points to the StreamTag data
  delete m_streamTagData.release();

  // Fill the stream tags
  uint32_t nStreamTagWords = eformat::helper::size_word(hltResult->getStreamTags());
  m_streamTagData = std::make_unique<uint32_t[]>(nStreamTagWords);
  try {
    // encode can throw exceptions if the encoding fails
    eformat::helper::encode(hltResult->getStreamTags(),nStreamTagWords,m_streamTagData.get());
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
  re->stream_tag(nStreamTagWords, m_streamTagData.get());

  // Fill the HLT bits
  const std::vector<uint32_t>& hltBits = hltResult->getHltBitsAsWords();
  re->hlt_info(hltBits.size(), hltBits.data());

  // Read the stream tags to decide which HLT ROBFragments to write out
  std::set<eformat::helper::SourceIdentifier> resultIdsToWrite;
  for (const eformat::helper::StreamTag& st : hltResult->getStreamTags()) {
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

  // Clear the FEA stack
  m_fullEventAssembler.clear();

  // Loop over the module IDs and fill the ROBFragments
  ATH_MSG_DEBUG("Iterating over " << resultIdsToWrite.size() << " HLT result IDs to assemble output data");
  const std::unordered_map<uint16_t, std::vector<uint32_t>>& serialisedData = hltResult->getSerialisedData();
  for (const eformat::helper::SourceIdentifier& resultId : resultIdsToWrite) {
    // Find the serialised data for this module ID
    const auto it = serialisedData.find(resultId.module_id());
    if (it==serialisedData.end()) {
      ATH_MSG_ERROR("HLT result with ID 0x" << MSG::hex << resultId.code() << MSG::dec
                    << " requested by a stream tag, but missing in the serialised data");
      return StatusCode::FAILURE;
    }
    const std::vector<uint32_t>& data = it->second;

    // Get a pointer to ROD data vector to be filled
    std::vector<uint32_t>* rod = m_fullEventAssembler.getRodData(resultId.code());
    if (!rod) {
      ATH_MSG_ERROR("Failed to get RODDATA pointer for HLT result ID 0x" << MSG::hex << resultId.code() << MSG::dec);
      return StatusCode::FAILURE;
    }

    // Fill the ROD data vector
    rod->assign(data.cbegin(), data.cend());
    ATH_MSG_DEBUG("Assembled data for HLT result ID 0x" << MSG::hex << resultId.code() << MSG::dec << " with "
                  << data.size() << " words of serialised payload");
  }

  ATH_MSG_DEBUG("Appending data to RawEventWrite");
  // Create ROBFragments and append them to the RawEventWrite
  m_fullEventAssembler.fill(re, msg());

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
