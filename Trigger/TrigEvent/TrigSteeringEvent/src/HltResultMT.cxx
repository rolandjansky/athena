/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/HLTResultMT.h"

// =============================================================================
// Standard constructor
// =============================================================================
HLT::HLTResultMT::HLTResultMT(std::vector<eformat::helper::StreamTag> streamTags,
                         std::vector<uint32_t> hltBits,
                         std::unordered_map<uint16_t, std::vector<uint32_t> > data)
: m_streamTags(streamTags),
  m_hltBits(hltBits),
  m_data(data) {}

// =============================================================================
// Standard destructor
// =============================================================================
HLT::HLTResultMT::~HLTResultMT() {}

// =============================================================================
// Getter/setter methods for stream tags
// =============================================================================
const std::vector<eformat::helper::StreamTag>& HLT::HLTResultMT::getStreamTags() const {
  return m_streamTags;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setStreamTags(const std::vector<eformat::helper::StreamTag>& streamTags) {
  // copy assignment
  m_streamTags = streamTags;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::addStreamTag(const eformat::helper::StreamTag& streamTag) {
  m_streamTags.push_back(streamTag);
}

// =============================================================================
// Getter/setter methods for trigger bits
// =============================================================================
const std::vector<uint32_t>& HLT::HLTResultMT::getHltBits() const {
  return m_hltBits;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setHltBits(const std::vector<uint32_t>& bits) {
  // copy assignment
  m_hltBits = bits;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::addHltBitsWord(const uint32_t& word) {
  m_hltBits.push_back(word);
}

// =============================================================================
// Getter/setter methods for serialised data
// =============================================================================
/// Serialised data getter
const std::unordered_map<uint16_t, std::vector<uint32_t> >& HLT::HLTResultMT::getSerialisedData() const {
  return m_data;
}

// -----------------------------------------------------------------------------
const std::vector<uint32_t>& HLT::HLTResultMT::getSerialisedData(uint16_t moduleId) const {
  // this can throw std::out_of_range - implement messaging (std::cerr or ATH_MSG_ERR) to avoid exception
  return m_data.at(moduleId);
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::setSerialisedData(const std::unordered_map<uint16_t, std::vector<uint32_t> >& data) {
  // copy assignment (WARNING, data may be large!)
  m_data = data;
}

// -----------------------------------------------------------------------------
void HLT::HLTResultMT::addSerialisedData(const uint16_t moduleId, const std::vector<uint32_t>& data) {
  if (m_data.find(moduleId)!=m_data.cend()) {
    // implement error printout and handling here !!! use either std::cerr or ATH_MSG_ERR
  }
  else {
    m_data[moduleId] = data;
  }
}
