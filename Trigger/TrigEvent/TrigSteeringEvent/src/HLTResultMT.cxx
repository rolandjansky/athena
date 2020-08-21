/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResultMT.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include <algorithm>
#include <utility>

#define CONTEXT_NAME "HLT::HLTResultMT"

// =============================================================================
// Standard constructor
// =============================================================================
HLT::HLTResultMT::HLTResultMT(std::vector<eformat::helper::StreamTag> streamTags,
                              boost::dynamic_bitset<uint32_t> hltPassRawBits,
                              boost::dynamic_bitset<uint32_t> hltPrescaledBits,
                              boost::dynamic_bitset<uint32_t> hltRerunBits,
                              std::unordered_map<uint16_t, std::vector<uint32_t> > data,
                              std::vector<uint32_t> status,
                              std::set<uint16_t> truncatedModuleIds)
: m_streamTags(std::move(streamTags)),
  m_hltPassRawBits(std::move(hltPassRawBits)),
  m_hltPrescaledBits(std::move(hltPrescaledBits)),
  m_hltRerunBits(std::move(hltRerunBits)),
  m_data(std::move(data)),
  m_status(std::move(status)),
  m_truncatedModuleIds(std::move(truncatedModuleIds)) {}

// =============================================================================
// Getter/setter methods for stream tags
// =============================================================================
const std::vector<eformat::helper::StreamTag>& HLT::HLTResultMT::getStreamTags() const {
  return m_streamTags;
}

// -----------------------------------------------------------------------------
std::vector<eformat::helper::StreamTag>& HLT::HLTResultMT::getStreamTagsNonConst() {
  return m_streamTags;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::setStreamTags(const std::vector<eformat::helper::StreamTag>& streamTags) {
  m_streamTags.clear();
  for (const auto& st : streamTags) {
    ATH_CHECK(addStreamTag(st));
  }
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addStreamTag(const eformat::helper::StreamTag& streamTag) {
  // Check if a stream tag with the same type and name is already in the result
  auto compareTypeName = [&streamTag](const eformat::helper::StreamTag& existingStreamTag) {
    return streamTag.type == existingStreamTag.type && streamTag.name == existingStreamTag.name;
  };
  auto p = std::find_if(m_streamTags.begin(), m_streamTags.end(), compareTypeName);

  // In case of duplicate, merge ROBs and SubDets, otherwise just append the tag to the result
  if (p != m_streamTags.end()) {
    if (streamTag.obeys_lumiblock != p->obeys_lumiblock) {
      ATH_REPORT_ERROR_WITH_CONTEXT(StatusCode::FAILURE, CONTEXT_NAME)
        << "Stream tags have equal type and name (" << streamTag.type << "/" << streamTag.name
        << "), but inconsistent obeys_lumiblock flag";
      return StatusCode::FAILURE;
    }
    p->robs.insert(streamTag.robs.begin(), streamTag.robs.end());
    p->dets.insert(streamTag.dets.begin(), streamTag.dets.end());
  }
  else {
    m_streamTags.push_back(streamTag);
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Getter/setter methods for trigger bits
// =============================================================================
const boost::dynamic_bitset<uint32_t>& HLT::HLTResultMT::getHltPassRawBits() const {
  return m_hltPassRawBits;
}

// -----------------------------------------------------------------------------
const boost::dynamic_bitset<uint32_t>& HLT::HLTResultMT::getHltPrescaledBits() const {
  return m_hltPrescaledBits;
}

// -----------------------------------------------------------------------------
const boost::dynamic_bitset<uint32_t>& HLT::HLTResultMT::getHltRerunBits() const {
  return m_hltRerunBits;
}

// -----------------------------------------------------------------------------
const std::vector<uint32_t>& HLT::HLTResultMT::getHltBitsAsWords() {
  m_hltBitWords.clear();
  if (m_hltPassRawBits.num_blocks() != m_hltPrescaledBits.num_blocks() || m_hltPassRawBits.num_blocks() != m_hltRerunBits.num_blocks()) {
    throw std::runtime_error("Must have the same number of bits in m_hltPassRawBits, m_hltPrescaledBits and m_hltRerunBits.");
  }
  m_hltBitWords.resize(m_hltPassRawBits.num_blocks() + m_hltPrescaledBits.num_blocks() + m_hltRerunBits.num_blocks());
  boost::to_block_range(m_hltPassRawBits, m_hltBitWords.begin());
  boost::to_block_range(m_hltPrescaledBits, m_hltBitWords.begin() + m_hltPassRawBits.num_blocks());
  boost::to_block_range(m_hltRerunBits, m_hltBitWords.begin() + m_hltPassRawBits.num_blocks() + m_hltPrescaledBits.num_blocks());
  return m_hltBitWords;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setHltPassRawBits(const boost::dynamic_bitset<uint32_t>& bitset) {
  // copy assignment
  m_hltPassRawBits = bitset;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setHltPrescaledBits(const boost::dynamic_bitset<uint32_t>& bitset) {
  // copy assignment
  m_hltPrescaledBits = bitset;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setHltRerunBits(const boost::dynamic_bitset<uint32_t>& bitset) {
  // copy assignment
  m_hltRerunBits = bitset;
}

// =============================================================================
// Getter/setter methods for serialised data
// =============================================================================
const std::unordered_map<uint16_t, std::vector<uint32_t> >& HLT::HLTResultMT::getSerialisedData() const {
  return m_data;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::getSerialisedData(const uint16_t moduleId, const std::vector<uint32_t>*& data) const {
  data = nullptr;
  const auto it = m_data.find(moduleId);
  if (it==m_data.cend()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG, CONTEXT_NAME)
      << "No data available in the stored map for the requested moduleId=" << moduleId;
    return StatusCode::FAILURE;
  }
  else {
    data = &(it->second);
  }
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setSerialisedData(const std::unordered_map<uint16_t, std::vector<uint32_t> >& data) {
  // WARNING, copying data which may be large
  m_data = data;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::addSerialisedData(const uint16_t moduleId, const std::vector<uint32_t>& data) {
    std::vector<uint32_t>& v = m_data[moduleId]; // creates new empty vector if the key doesn't exist
    // WARNING, copying data which may be large
    v.insert(v.end(),data.begin(),data.end());
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addSerialisedDataWithCheck(const uint16_t moduleId, const std::vector<uint32_t>& data) {
  if (m_data.find(moduleId)!=m_data.cend()) {
    ATH_REPORT_ERROR_WITH_CONTEXT(StatusCode::FAILURE, CONTEXT_NAME)
      << "Trying to add data for a module which already exists in the stored map, moduleId=" << moduleId;
    return StatusCode::FAILURE;
  }
  else {
    // WARNING, copying data which may be large
    m_data[moduleId] = data;
  }
  return StatusCode::SUCCESS;
}

// =============================================================================
// Getter/setter methods for status words
// =============================================================================
const std::vector<uint32_t>& HLT::HLTResultMT::getStatus() const {
  return m_status;
}

// -----------------------------------------------------------------------------
const eformat::helper::Status HLT::HLTResultMT::getFirstStatusWord() const {
  if (m_status.empty())
    return eformat::helper::Status(eformat::GenericStatus::UNCLASSIFIED,0); // empty status word
  else
    return eformat::helper::Status(m_status.at(0));
}

// -----------------------------------------------------------------------------
const std::vector<HLT::OnlineErrorCode> HLT::HLTResultMT::getErrorCodes() const {
  std::vector<HLT::OnlineErrorCode> errorCodes;
  if (m_status.size()<2)
    return errorCodes;
  for (auto it=m_status.cbegin()+1; it!=m_status.cend(); ++it) {
    errorCodes.push_back(static_cast<HLT::OnlineErrorCode>(*it));
  }
  return errorCodes;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setStatus(const std::vector<uint32_t>& status) {
  // copy assignment
  m_status = status;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setErrorCodes(const std::vector<HLT::OnlineErrorCode>& errorCodes,
                                     const eformat::helper::Status& firstStatusWord) {
  m_status.clear();
  m_status.push_back(firstStatusWord.code());
  for (const HLT::OnlineErrorCode& code : errorCodes)
    m_status.push_back(static_cast<uint32_t>(code));
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::addErrorCode(const HLT::OnlineErrorCode& errorCode,
                                    const eformat::helper::Status& firstStatusWord) {
  if (m_status.empty()) m_status.push_back(firstStatusWord.code());
  else m_status[0] |= firstStatusWord.code();
  m_status.push_back(static_cast<uint32_t>(errorCode));
}

// =============================================================================
// Getter/setter methods for truncation information
// =============================================================================
const std::set<uint16_t>& HLT::HLTResultMT::getTruncatedModuleIds() const {
  return m_truncatedModuleIds;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::addTruncatedModuleId(const uint16_t moduleId) {
  m_truncatedModuleIds.insert(moduleId);
}

// =============================================================================
std::ostream& operator<<(std::ostream& str, const HLT::HLTResultMT& hltResult) {
  auto printWord = [&str](const uint32_t word, const size_t width=8){
    str << "0x" << std::hex << std::setw(width) << std::setfill('0') << word << " " << std::dec;
  };
  str << "Printing HLTResultMT:" << std::endl;

  // Status
  str << "--> Status words = ";
  for (const uint32_t word : hltResult.getStatus()) {
    printWord(word);
  }
  str << std::endl;

  // Stream tags
  str << "--> Stream tags  = ";
  bool first = true;
  for (const eformat::helper::StreamTag& st : hltResult.getStreamTags()) {
    if (first) first=false;
    else str << "                   ";
    str << "{" << st.name << ", " << st.type << ", obeysLB=" << st.obeys_lumiblock << ", robs=[";
    for (const auto& robid : st.robs) printWord(robid);
    str << "], dets = [";
    for (const auto& detid : st.dets) printWord(detid,2);
    str << "]}" << std::endl;
  }
  if (hltResult.getStreamTags().empty()) str << std::endl;

  // HLT bits
  std::vector<uint32_t> hltPassRawBitWords;
  std::vector<uint32_t> hltPrescaledBitWords;
  std::vector<uint32_t> hltRerunBitWords;
  hltPassRawBitWords.resize(hltResult.getHltPassRawBits().num_blocks());
  hltPrescaledBitWords.resize(hltResult.getHltPrescaledBits().num_blocks());
  hltRerunBitWords.resize(hltResult.getHltRerunBits().num_blocks());
  boost::to_block_range(hltResult.getHltPassRawBits(),hltPassRawBitWords.begin());
  boost::to_block_range(hltResult.getHltPrescaledBits(),hltPrescaledBitWords.begin());
  boost::to_block_range(hltResult.getHltRerunBits(),hltRerunBitWords.begin());
  str << "--> HLT bits     = ";
  for (const uint32_t word : hltPassRawBitWords) {
    printWord(word);
  }
  for (const uint32_t word : hltPrescaledBitWords) {
    printWord(word);
  }
  for (const uint32_t word : hltRerunBitWords) {
    printWord(word);
  }
  str << std::endl;

  // Payload size
  str << "--> Payload size = ";
  first = true;
  for (const auto& p : hltResult.getSerialisedData()) {
    if (first) first=false;
    else str << "                   ";
    str << "{module " << p.first << ": " << p.second.size() << " words}" << std::endl;
  }

  // Truncation information
  if (!hltResult.getTruncatedModuleIds().empty()) {
    str << "--> Truncated results = ";
    first = true;
    for (const uint16_t moduleId : hltResult.getTruncatedModuleIds()) {
      if (first) first=false;
      else str << ", ";
      str << moduleId;
    }
    str << std::endl;
  }

  return str;
}
