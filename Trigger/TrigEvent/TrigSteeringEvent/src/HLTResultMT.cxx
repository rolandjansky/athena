/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResultMT.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include <algorithm>

#define CONTEXT_NAME "HLT::HLTResultMT"

// =============================================================================
// Standard constructor
// =============================================================================
HLT::HLTResultMT::HLTResultMT(std::vector<eformat::helper::StreamTag> streamTags,
                              boost::dynamic_bitset<uint32_t> hltPassRawBits,
                              boost::dynamic_bitset<uint32_t> hltPrescaledBits,
                              boost::dynamic_bitset<uint32_t> hltRerunBits,
                              std::unordered_map<uint16_t, std::vector<uint32_t> > data,
                              std::vector<uint32_t> status)
: m_streamTags(streamTags),
  m_hltPassRawBits(hltPassRawBits),
  m_hltPrescaledBits(hltPrescaledBits),
  m_hltRerunBits(hltRerunBits),
  m_data(data),
  m_status(status) {}

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
  boost::to_block_range(m_hltPrescaledBits, m_hltBitWords.begin());
  boost::to_block_range(m_hltRerunBits, m_hltBitWords.begin());
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

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::reserveHltBits(size_t numberOfChains) {
  try {
    if (m_hltPassRawBits.size() <= numberOfChains) m_hltPassRawBits.resize(numberOfChains+1);
    if (m_hltPrescaledBits.size() <= numberOfChains) m_hltPrescaledBits.resize(numberOfChains+1);
    if (m_hltRerunBits.size() <= numberOfChains) m_hltRerunBits.resize(numberOfChains+1);
  }
  catch (const std::exception& ex) {
    ATH_REPORT_ERROR_WITH_CONTEXT(StatusCode::FAILURE, CONTEXT_NAME)
      << "Failed to reserve space for " << numberOfChains << " HLT bits, likely memory allocation failed. std::exception caught: " << ex.what();
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_REPORT_ERROR_WITH_CONTEXT(StatusCode::FAILURE, CONTEXT_NAME)
      << "Failed to  reserve space for " << numberOfChains << " HLT bits, likely memory allocation failed. Unknown exception caught.";
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addHltPassRawBits(const std::vector<size_t>& indices) {
  for (const size_t index : indices) {
    ATH_CHECK(addHltPassRawBit(index));
  }
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addHltPrescaledBits(const std::vector<size_t>& indices) {
  for (const size_t index : indices) {
    ATH_CHECK(addHltPrescaledBit(index));
  }
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addHltRerunBits(const std::vector<size_t>& indices) {
  for (const size_t index : indices) {
    ATH_CHECK(addHltRerunBit(index));
  }
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addHltPassRawBit(size_t index) {
  ATH_CHECK(addHltBitInternal(index, m_hltPassRawBits));
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addHltPrescaledBit(size_t index) {
  ATH_CHECK(addHltBitInternal(index, m_hltPrescaledBits));
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addHltRerunBit(size_t index) {
  ATH_CHECK(addHltBitInternal(index, m_hltRerunBits));
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------
StatusCode HLT::HLTResultMT::addHltBitInternal(size_t index, boost::dynamic_bitset<uint32_t>& bitset) {
  try {
    bitset.set(index);
  }
  catch (const std::exception& ex) {
    ATH_REPORT_ERROR_WITH_CONTEXT(StatusCode::FAILURE, CONTEXT_NAME)
      << "Failed to add HLT bit, likely memory allocation failed. Was reserveHltBits() called? std::exception caught: " << ex.what();
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_REPORT_ERROR_WITH_CONTEXT(StatusCode::FAILURE, CONTEXT_NAME)
      << "Failed to add HLT bit, likely memory allocation failed. Was reserveHltBits() called? Unknown exception caught.";
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
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
const std::vector<uint32_t> HLT::HLTResultMT::getErrorCodes() const {
  if (m_status.size()<2)
    return {};
  else
    return {m_status.begin()+1, m_status.end()};
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setStatus(const std::vector<uint32_t>& status) {
  // copy assignment
  m_status = status;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setErrorCodes(const std::vector<uint32_t>& errorCodes,
                                     const eformat::helper::Status firstStatusWord) {
  m_status.clear();
  m_status.push_back(firstStatusWord.code());
  m_status.insert(m_status.end(),errorCodes.cbegin(),errorCodes.cend());
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::addErrorCode(const uint32_t& errorCode,
                                    const eformat::helper::Status firstStatusWord) {
  if (m_status.empty()) m_status.push_back(firstStatusWord.code());
  else m_status[0] |= firstStatusWord.code();
  m_status.push_back(errorCode);
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

  return str;
}
