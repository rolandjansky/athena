/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOutputHandling/HLTResultMT.h"
#include <algorithm>

// =============================================================================
// Standard constructor
// =============================================================================
HLTResultMT::HLTResultMT(std::vector<eformat::helper::StreamTag> streamTags,
                         std::vector<uint32_t> hltBits,
                         std::unordered_map<uint16_t, std::vector<uint32_t> > data,
                         std::vector<uint32_t> status)
: m_streamTags(streamTags),
  m_hltBits(hltBits),
  m_data(data),
  m_status(status) {}

// =============================================================================
// Copy constructor
// =============================================================================
HLTResultMT::HLTResultMT(const HLTResultMT& other)
: m_streamTags(other.m_streamTags),
  m_hltBits(other.m_hltBits),
  m_data(other.m_data),
  m_status(other.m_status) {}

// =============================================================================
// Standard destructor
// =============================================================================
HLTResultMT::~HLTResultMT() {}

// =============================================================================
// Getter/setter methods for stream tags
// =============================================================================
const std::vector<eformat::helper::StreamTag>& HLTResultMT::getStreamTags() const {
  return m_streamTags;
}

// -----------------------------------------------------------------------------
void HLTResultMT::setStreamTags(const std::vector<eformat::helper::StreamTag>& streamTags) {
  // copy assignment
  m_streamTags = streamTags;
}

// -----------------------------------------------------------------------------
void HLTResultMT::addStreamTag(const eformat::helper::StreamTag& streamTag) {
  // Check if a stream tag with the same type and name is already in the result
  auto compareTypeName = [&streamTag](const eformat::helper::StreamTag& existingStreamTag) {
    return streamTag.type == existingStreamTag.type && streamTag.name == existingStreamTag.name;
  };
  auto p = std::find_if(m_streamTags.begin(), m_streamTags.end(), compareTypeName);

  // In case of duplicate, merge ROBs and SubDets, otherwise just append the tag to the result
  if (p != m_streamTags.end()) {
    if (streamTag.obeys_lumiblock != p->obeys_lumiblock) {
      // This shouldn't happen, but in case it does, true takes precedence
      p->obeys_lumiblock = true;
    }
    p->robs.insert(streamTag.robs.begin(), streamTag.robs.end());
    p->dets.insert(streamTag.dets.begin(), streamTag.dets.end());
  }
  else {
    m_streamTags.push_back(streamTag);
  }
}

// =============================================================================
// Getter/setter methods for trigger bits
// =============================================================================
const std::vector<uint32_t>& HLTResultMT::getHltBits() const {
  return m_hltBits;
}

// -----------------------------------------------------------------------------
void HLTResultMT::setHltBits(const std::vector<uint32_t>& bits) {
  // copy assignment
  m_hltBits = bits;
}

// -----------------------------------------------------------------------------
void HLTResultMT::addHltBitsWord(const uint32_t& word) {
  m_hltBits.push_back(word);
}

// =============================================================================
// Getter/setter methods for serialised data
// =============================================================================
/// Serialised data getter
const std::unordered_map<uint16_t, std::vector<uint32_t> >& HLTResultMT::getSerialisedData() const {
  return m_data;
}

// -----------------------------------------------------------------------------
const std::vector<uint32_t>& HLTResultMT::getSerialisedData(uint16_t moduleId) const {
  // this can throw std::out_of_range - implement messaging (std::cerr or ATH_MSG_ERR) to avoid exception
  return m_data.at(moduleId);
}

// -----------------------------------------------------------------------------
void HLTResultMT::setSerialisedData(const std::unordered_map<uint16_t, std::vector<uint32_t> >& data) {
  // copy assignment (WARNING, data may be large!)
  m_data = data;
}

// -----------------------------------------------------------------------------
void HLTResultMT::addSerialisedData(const uint16_t moduleId, const std::vector<uint32_t>& data) {
  if (m_data.find(moduleId)!=m_data.cend()) {
    // implement error printout and handling here !!! use either std::cerr or ATH_MSG_ERR
  }
  else {
    m_data[moduleId] = data;
  }
}

// =============================================================================
// Getter/setter methods for status words
// =============================================================================
const std::vector<uint32_t>& HLTResultMT::getStatus() const {
  return m_status;
}

// -----------------------------------------------------------------------------
void HLTResultMT::setStatus(const std::vector<uint32_t>& status) {
  // copy assignment
  m_status = status;
}

// -----------------------------------------------------------------------------
void HLTResultMT::addStatusWord(const uint32_t& word) {
  m_status.push_back(word);
}
