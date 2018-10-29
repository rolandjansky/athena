/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGOUTPUTHANDLING_HLTRESULTMT_H
#define TRIGOUTPUTHANDLING_HLTRESULTMT_H

#include "AthenaKernel/CLASS_DEF.h"
#include "eformat/StreamTag.h"
#include <vector>
#include <unordered_map>

/** @class HLTResultMT
 *  @brief A container class for data required to build online output from HLT.
 *
 *  It is not intended to modify the stored data, but may perform extra checks and debug printouts in the
 *  getter/setter methods.
 **/
class HLTResultMT {
public:
  /// Standard constructor
  HLTResultMT(std::vector<eformat::helper::StreamTag> streamTags = {},
              std::vector<uint32_t> hltBits = {},
              std::unordered_map<uint16_t, std::vector<uint32_t> > data = {},
              std::vector<uint32_t> status = {});
  /// Copy constructor
  HLTResultMT(const HLTResultMT& other);
  /// Standard destructor
  virtual ~HLTResultMT();

  /// Stream tags getter
  const std::vector<eformat::helper::StreamTag>& getStreamTags() const;
  /// Stream tags setter
  void setStreamTags(const std::vector<eformat::helper::StreamTag>& streamTags);
  /// Append a stream tag
  void addStreamTag(const eformat::helper::StreamTag& streamTag);

  /// HLT bits getter
  const std::vector<uint32_t>& getHltBits() const;
  /// HLT bits setter
  void setHltBits(const std::vector<uint32_t>& bits);
  /// Append a word with HLT bits
  void addHltBitsWord(const uint32_t& word);

  /// Serialised data getter
  const std::unordered_map<uint16_t, std::vector<uint32_t> >& getSerialisedData() const;
  /// Serialised data getter for a given module ID
  const std::vector<uint32_t>& getSerialisedData(uint16_t moduleId) const;
  /// Serialised data setter
  void setSerialisedData(const std::unordered_map<uint16_t, std::vector<uint32_t> >& data);
  /// Append serialised data for a given module ID
  void addSerialisedData(const uint16_t moduleId, const std::vector<uint32_t>& data);

  /// Status words getter
  const std::vector<uint32_t>& getStatus() const;
  /// Status words setter
  void setStatus(const std::vector<uint32_t>& status);
  /// Append a status word
  void addStatusWord(const uint32_t& word);

private:
  /// Stream tags of the event
  std::vector<eformat::helper::StreamTag> m_streamTags;
  /// Serialised HLT bits (flagging which chains passed)
  std::vector<uint32_t> m_hltBits;
  /// Serialised result (ROBFragment payload) for each module ID (0 for full result, >0 for data scouting)
  std::unordered_map<uint16_t, std::vector<uint32_t> > m_data;
  /// Event status words (used to indicate processing errors)
  std::vector<uint32_t> m_status;
};

CLASS_DEF(HLTResultMT, 69657051, 1)

#endif // TRIGOUTPUTHANDLING_HLTRESULTMT_H
