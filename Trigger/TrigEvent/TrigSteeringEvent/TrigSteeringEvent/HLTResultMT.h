/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERINGEVENT_HLTResultMT_H
#define TRIGSTEERINGEVENT_HLTResultMT_H

// Trigger includes
#include "TrigSteeringEvent/OnlineErrorCode.h"

// Athena includes
#include "AthenaKernel/CLASS_DEF.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"

// TDAQ includes
#include "eformat/Status.h"
#include "eformat/StreamTag.h"

// System includes
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <unordered_map>

namespace HLT {

  /** @class HLTResultMT
   *  @brief A container class for data required to build online output from HLT
   *
   *  This is not an EDM class and it is not meant to be serialised and persistified. It is a transient container for
   *  collecting serialised EDM objects and other event data before saving to ByteStream or after reading from
   *  ByteStream. It provides only getter and setter methods for the stored data. It does not serialise/deserialise
   *  the data on its own.
   **/
  class HLTResultMT {
  public:
    /// Standard constructor
    HLTResultMT(std::vector<eformat::helper::StreamTag> streamTags = {},
                boost::dynamic_bitset<uint32_t> hltPassRawBits = boost::dynamic_bitset<uint32_t>(),
                boost::dynamic_bitset<uint32_t> hltPrescaledBits = boost::dynamic_bitset<uint32_t>(),
                boost::dynamic_bitset<uint32_t> hltRerunBits = boost::dynamic_bitset<uint32_t>(),
                std::unordered_map<uint16_t, std::vector<uint32_t> > data = {},
                std::vector<uint32_t> status = {0},
                std::set<uint16_t> truncatedModuleIds = {});


    // ------------------------- Stream tags getters/setters -------------------

    /// Const-getter for stream tags
    const std::vector<eformat::helper::StreamTag>& getStreamTags() const;

    /// Non-const-getter for stream tags needed by the result maker to remove disabled ROBs/SubDets
    std::vector<eformat::helper::StreamTag>& getStreamTagsNonConst();

    /** @brief Replace the stored list of stream tags with the given one
     *
     *  Removes duplicates from the input list by merging robs and dets lists of stream tags with equal name and type.
     *  @return FAILURE only if two stream tags with equal name nad type have inconsistent obeys_lumiblock flags.
     **/
    StatusCode setStreamTags(const std::vector<eformat::helper::StreamTag>& streamTags);

    /** @brief Append one stream tag to the stored list
     *
     *  Removes duplicates by merging robs and dets lists of stream tags with equal name and type.
     *  @return FAILURE only if two stream tags with equal name nad type have inconsistent obeys_lumiblock flags.
     **/
    StatusCode addStreamTag(const eformat::helper::StreamTag& streamTag);


    // ------------------------- HLT bits getters/setters ----------------------

    /// Const-getter for HLT pass raw bits
    const boost::dynamic_bitset<uint32_t>& getHltPassRawBits() const;

    /// Const-getter for HLT prescaled bits
    const boost::dynamic_bitset<uint32_t>& getHltPrescaledBits() const;

    /// Const-getter for HLT rerun bits
    const boost::dynamic_bitset<uint32_t>& getHltRerunBits() const;

    /// Const-getter for HLT bits as uint32_t array. Ordering: PassRaw, Prescaled, Rerun.
    const std::vector<uint32_t>& getHltBitsAsWords();

    /// Replace HLT pass raw bits with the given bitset
    void setHltPassRawBits(const boost::dynamic_bitset<uint32_t>& bitset);

    /// Replace HLT prescaled bits with the given bitset
    void setHltPrescaledBits(const boost::dynamic_bitset<uint32_t>& bitset);

    /// Replace HLT rerun raw bits with the given bitset
    void setHltRerunBits(const boost::dynamic_bitset<uint32_t>& bitset);


    // ------------------------- Serialised data getters/setters ---------------

    /** @brief Serialised data getter
     *
     *  The only way to retrieve the data by reference with no exception throwing.
     **/
    const std::unordered_map<uint16_t, std::vector<uint32_t> >& getSerialisedData() const;

    /** @brief Serialised data getter for a given moduleId
     *  @in moduleId
     *  @out data - const pointer to the stored data, nullptr on failure
     *  @return FAILURE if moduleId is not found in the stored map
     **/
    StatusCode getSerialisedData(const uint16_t moduleId, const std::vector<uint32_t>*& data) const;

    /// Replaces serialised data with a copy of the given data
    void setSerialisedData(std::unordered_map<uint16_t, std::vector<uint32_t> > data);

    /// Append serialised data (copy of input) for a given moduleId, doesn't remove existing data
    void addSerialisedData(const uint16_t moduleId, const std::vector<uint32_t>& data);

    /** @brief Add serialised data for a given moduleId. Fails if any data for the given module already exist
     *  @return FAILURE if moduleId is already present in the stored map
     **/
    StatusCode addSerialisedDataWithCheck(const uint16_t moduleId, std::vector<uint32_t> data);


    // ------------------------- Error codes getters/setters -------------------
    // The event processing status is stored as one bit-mask word corresponding to eformat::helper::Status
    // and n optional error codes. Online HLT framework uses them to store HLT::OnlineErrorCode.

    /// Full event status reference getter (1 bit-mask status word + error code words)
    const std::vector<uint32_t>& getStatus() const;

    /// First status word getter (by value)
    const eformat::helper::Status getFirstStatusWord() const;

    /// Error codes getter  (by value) - strips off the first bit-mask status word
    const std::vector<HLT::OnlineErrorCode> getErrorCodes() const;

    /// Replace the full status words with the given data
    void setStatus(std::vector<uint32_t> status);

    /// Replace error codes with the given codes
    void setErrorCodes(const std::vector<HLT::OnlineErrorCode>& errorCodes,
                       const eformat::helper::Status firstStatusWord = {
                         eformat::GenericStatus::DATA_CORRUPTION,
                         eformat::FullEventStatus::PSC_PROBLEM
                       });

    /** @brief Append an error code
     *
     *  Makes the current first word |= new first word and appends errorCode
     *  to the vector of optional error codes
     **/
    void addErrorCode(const HLT::OnlineErrorCode& errorCode,
                      const eformat::helper::Status firstStatusWord = {
                        eformat::GenericStatus::DATA_CORRUPTION,
                        eformat::FullEventStatus::PSC_PROBLEM
                      });


    // ------------------------- Truncation information ------------------------

    /// Getter for the truncation information
    const std::set<uint16_t>& getTruncatedModuleIds() const;

    /// Add module ID to the list of truncated results
    void addTruncatedModuleId(const uint16_t moduleId);

    // ------------------------- Extra helper methods --------------------------
    /// If there is at least one stream tag set, it means the event is accepted
    bool isAccepted() const {return !m_streamTags.empty();}

  private:
    // ------------------------- Private data members --------------------------

    /// Stream tags of the event
    std::vector<eformat::helper::StreamTag> m_streamTags;

    /// HLT bits (flagging which chains passed)
    boost::dynamic_bitset<uint32_t> m_hltPassRawBits;
    boost::dynamic_bitset<uint32_t> m_hltPrescaledBits;
    boost::dynamic_bitset<uint32_t> m_hltRerunBits;

    /** @brief Vector storing m_hltBits converted to 4-byte words
     *
     *  HLTResultMT needs to own this vector because its lifetime has to be ensured until the serialisation is finished.
     *  This vector is updated internally from m_hltPassRawBits, m_hltPrescaledBits, m_hltRerunBits and does not have a setter method.
     **/
    std::vector<uint32_t> m_hltBitWords;

    /// Serialised result (ROBFragment payload) for each moduleId (0 for full result, >0 for data scouting)
    std::unordered_map<uint16_t, std::vector<uint32_t> > m_data;

    /// First word is eformat::helper::Status, next words are optional error codes
    std::vector<uint32_t> m_status;

    /// List of module IDs with truncation. Used only by the framework while creating the result.
    /// It is not stored in ByteStream files.
    std::set<uint16_t> m_truncatedModuleIds;
  };
} // namespace HLT

/// operator<< overload for printing to std::ostream
std::ostream& operator<<(std::ostream& str, const HLT::HLTResultMT& hltResult);

CLASS_DEF(HLT::HLTResultMT, 172156324, 1)

#endif // TRIGSTEERINGEVENT_HLTResultMT_H
