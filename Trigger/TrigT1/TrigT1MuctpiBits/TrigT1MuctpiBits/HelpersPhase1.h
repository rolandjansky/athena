/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1MUCTPIBITS_HELPERSPHASE1_H
#define TRIGT1MUCTPIBITS_HELPERSPHASE1_H

#include "MuCTPI_Bits.h"
#include <vector>
#include <cstdint>
#include <exception>

namespace LVL1::MuCTPIBits {
  // Helper types
  enum class WordType : uint8_t {Undefined=0, Timeslice, Multiplicity, Candidate, Topo, Status, MAX};
  enum class SubsysID : uint8_t {Undefined=0, Barrel, Forward, Endcap, MAX};
  struct TimesliceHeader {
      uint16_t bcid{0};
      uint16_t tobCount{0};
      uint16_t candCount{0};
  };

  // Status data word error definitions
  static constexpr std::array<std::string_view,16> DataStatusWordErrors = {
    "Event number mismatch between MSPA and TRP in the central time slice",
    "Event number mismatch between MSPC and TRP in the central time slice",
    "Event number mismatch between MSPA and MSPC in any time slice",
    "BCID mismatch between TRP and MSPA in the central time slice",
    "BCID mismatch between TRP and MSPC in the central time slice",
    "BCID mismatch between MSPA and MSPC in any time slice",
    "MSPA multiplicity LVDS link CRC error in any time slice",
    "MSPC multiplicity LVDS link CRC error in any time slice",
    "Sector logic error flag set on any of the 104 MSPA sectors",
    "Sector logic error flag set on any of the 104 MSPC sectors",
    "Error flag set in any of the muon candidates in the event after zero-supression",
    "CRC error on the MSPA DAQ link (in any time slice)",
    "CRC error on the MSPC DAQ link (in any time slice)",
    "TriggerType reception timeout error",
    "MSPA DAQ link input FIFO full flag (cleared at EOF)",
    "MSPC DAQ link input FIFO full flag (cleared at EOF)"
  };

  // Helper functions
  /// Extract sub-word from 32-bit word by applying a shift and a mask
  inline constexpr uint32_t maskedWord(uint32_t word, uint32_t shift, uint32_t mask) {
    return ((word >> shift) & mask);
  }

  /// Extract sub-word from 64-bit word by applying a shift and a mask
  inline constexpr uint32_t maskedWord(uint64_t word, uint32_t shift, uint32_t mask) {
    return ((word >> shift) & mask);
  }

  /// Extract sub-word from 32-bit word by applying a shift and a mask
  inline constexpr uint32_t buildWord(uint32_t value, uint32_t shift, uint32_t mask) {
    return ((value & mask) << shift);
  }

  /// Compare a sub-word of a 32-bit word to an expected value
  inline constexpr bool wordEquals(uint32_t word, uint32_t shift, uint32_t mask, uint32_t value) {
    return maskedWord(word, shift, mask) == value;
  }

  /// Determine the type of a MUCTPI ROD word
  inline constexpr WordType getWordType(uint32_t word) {
    if (wordEquals(word, RUN3_TIMESLICE_MULT_WORD_ID_SHIFT, RUN3_TIMESLICE_MULT_WORD_ID_MASK, RUN3_TIMESLICE_MULT_WORD_ID_VAL)) {
      if (wordEquals(word, RUN3_TIMESLICE_MULT_WORD_NUM_SHIFT, RUN3_TIMESLICE_MULT_WORD_NUM_MASK, RUN3_TIMESLICE_WORD_NUM_VAL)) {
        return WordType::Timeslice;
      }
      return WordType::Multiplicity;
    } else if (wordEquals(word, RUN3_CAND_WORD_ID_SHIFT, RUN3_CAND_WORD_ID_MASK, RUN3_CAND_WORD_ID_VAL)) {
      return WordType::Candidate;
    } else if (wordEquals(word, RUN3_TOPO_WORD_ID_SHIFT, RUN3_TOPO_WORD_ID_MASK, RUN3_TOPO_WORD_ID_VAL)) {
      return WordType::Topo;
    } else if (wordEquals(word, RUN3_STATUS_WORD_ID_SHIFT, RUN3_STATUS_WORD_ID_MASK, RUN3_STATUS_WORD_ID_VAL)) {
      return WordType::Status;
    }
    return WordType::Undefined;
  }

  /// Decode timeslice word
  inline constexpr auto timesliceHeader(uint32_t word) {
    struct {
      uint32_t bcid{0};
      uint32_t tobCount{0};
      uint32_t candCount{0};
    } header;
    header.bcid = maskedWord(word, RUN3_TIMESLICE_BCID_SHIFT, RUN3_TIMESLICE_BCID_MASK);
    header.tobCount = maskedWord(word, RUN3_TIMESLICE_NTOB_SHIFT, RUN3_TIMESLICE_NTOB_MASK);
    header.candCount = maskedWord(word, RUN3_TIMESLICE_NCAND_SHIFT, RUN3_TIMESLICE_NCAND_MASK);
    return header;
  }

  /// Encode timeslice word
  inline constexpr uint32_t timesliceHeader(uint32_t bcid, uint32_t tobCount, uint32_t candCount) {
    uint32_t word{0};
    word |= buildWord(bcid, RUN3_TIMESLICE_BCID_SHIFT, RUN3_TIMESLICE_BCID_MASK);
    word |= buildWord(tobCount, RUN3_TIMESLICE_NTOB_SHIFT, RUN3_TIMESLICE_NTOB_MASK);
    word |= buildWord(candCount, RUN3_TIMESLICE_NCAND_SHIFT, RUN3_TIMESLICE_NCAND_MASK);
    return word;
  }

  /// Decode the index of the multitpicity word, which is 1, 2, or 3
  inline constexpr uint32_t multiplicityWordNumber(uint32_t word) {
    return maskedWord(word, RUN3_TIMESLICE_MULT_WORD_NUM_SHIFT, RUN3_TIMESLICE_MULT_WORD_NUM_MASK);
  }

  /// Encode the multiplicity words
  inline constexpr std::array<uint32_t,3> multiplicityWords(uint64_t multiplicity, uint32_t triggerBits, bool overflow) {
    std::array<uint32_t,3> words{}; // zero-initialised
    for (uint32_t iWord=0; iWord<words.size(); ++iWord) {
      words[iWord] |= buildWord(RUN3_TIMESLICE_MULT_WORD_ID_VAL, RUN3_TIMESLICE_MULT_WORD_ID_SHIFT, RUN3_TIMESLICE_MULT_WORD_ID_MASK);
      words[iWord] |= buildWord(iWord+1, RUN3_TIMESLICE_MULT_WORD_NUM_SHIFT, RUN3_TIMESLICE_MULT_WORD_NUM_MASK);
    }
    words[0] |= maskedWord(multiplicity, RUN3_MULTIPLICITY_PART1_SHIFT, RUN3_MULTIPLICITY_PART1_MASK);
    words[1] |= maskedWord(multiplicity, RUN3_MULTIPLICITY_PART2_SHIFT, RUN3_MULTIPLICITY_PART2_MASK);
    words[2] |= maskedWord(multiplicity, RUN3_MULTIPLICITY_PART3_SHIFT, RUN3_MULTIPLICITY_PART3_MASK);
    words[2] |= maskedWord(triggerBits, RUN3_MULTIPLICITY_TRIGBITS_SHIFT, RUN3_MULTIPLICITY_TRIGBITS_MASK);
    words[2] |= maskedWord(static_cast<uint32_t>(overflow), RUN3_MULTIPLICITY_OVERFLOW_SHIFT, RUN3_MULTIPLICITY_OVERFLOW_MASK);
    return words;
  }

  /// Decode the subsys ID from RoI candidate word (online or offline format, see discussion in ATR-25069)
  inline constexpr SubsysID getSubsysID(uint32_t word, bool onlineFormat) {
    if (onlineFormat) {
      if (wordEquals(word, RUN3_SUBSYS_ADDRESS_EC_SHIFT, RUN3_SUBSYS_ADDRESS_EC_MASK, RUN3_SUBSYS_ADDRESS_EC_VAL)) {
        return SubsysID::Endcap;
      } else if (wordEquals(word, RUN3_SUBSYS_ADDRESS_BAFW_SHIFT, RUN3_SUBSYS_ADDRESS_BAFW_MASK, RUN3_SUBSYS_ADDRESS_FW_VAL)) {
        return SubsysID::Forward;
      } else if (wordEquals(word, RUN3_SUBSYS_ADDRESS_BAFW_SHIFT, RUN3_SUBSYS_ADDRESS_BAFW_MASK, RUN3_SUBSYS_ADDRESS_BA_VAL)) {
        return SubsysID::Barrel;
      }
      return SubsysID::Undefined;
    }
    // offline format
    if (wordEquals(word, RUN3OFFLINE_SUBSYS_ADDRESS_EC_SHIFT, RUN3OFFLINE_SUBSYS_ADDRESS_EC_MASK, RUN3OFFLINE_SUBSYS_ADDRESS_EC_VAL)) {
      return SubsysID::Endcap;
    } else if (wordEquals(word, RUN3OFFLINE_SUBSYS_ADDRESS_BAFW_SHIFT, RUN3OFFLINE_SUBSYS_ADDRESS_BAFW_MASK, RUN3OFFLINE_SUBSYS_ADDRESS_FW_VAL)) {
      return SubsysID::Forward;
    } else if (wordEquals(word, RUN3OFFLINE_SUBSYS_ADDRESS_BAFW_SHIFT, RUN3OFFLINE_SUBSYS_ADDRESS_BAFW_MASK, RUN3OFFLINE_SUBSYS_ADDRESS_BA_VAL)) {
      return SubsysID::Barrel;
    }
    return SubsysID::Undefined;
  }

  /// Shift the candidate word sector ID subsystem/hemisphere bits from the online to the offline format (see ATR-25069)
  inline constexpr uint32_t convertSectorIDOnlineToOffline(uint32_t word) {
    uint32_t sectorID = maskedWord(word, RUN3_CAND_SECTORID_SHIFT, RUN3_CAND_SECTORID_MASK);
    uint32_t hemisphere = maskedWord(word, RUN3_SUBSYS_HEMISPHERE_SHIFT, RUN3_SUBSYS_HEMISPHERE_MASK);
    // Hemisphere is one bit, online 0 means side A, 1 means side C, offline it's the opposite
    hemisphere = hemisphere==0 ? 1 : 0;
    uint32_t theRestMask = ~buildWord(std::numeric_limits<uint32_t>::max(), RUN3_CAND_SECTOR_ADDRESS_SHIFT, RUN3_CAND_SECTOR_ADDRESS_MASK);
    uint32_t theRest = maskedWord(word, 0u, theRestMask);
    return theRest |
           buildWord(sectorID, RUN3OFFLINE_CAND_SECTORID_SHIFT, RUN3OFFLINE_CAND_SECTORID_MASK) |
           buildWord(hemisphere, RUN3OFFLINE_SUBSYS_HEMISPHERE_SHIFT, RUN3OFFLINE_SUBSYS_HEMISPHERE_MASK);
  }

  /// Shift the candidate word sector ID subsystem/hemisphere bits from the offline to the online format (see ATR-25069)
  inline constexpr uint32_t convertSectorIDOfflineToOnline(uint32_t word) {
    uint32_t sectorID = maskedWord(word, RUN3OFFLINE_CAND_SECTORID_SHIFT, RUN3OFFLINE_CAND_SECTORID_MASK);
    uint32_t hemisphere = maskedWord(word, RUN3OFFLINE_SUBSYS_HEMISPHERE_SHIFT, RUN3OFFLINE_SUBSYS_HEMISPHERE_MASK);
    // Hemisphere is one bit, online 0 means side A, 1 means side C, offline it's the opposite
    hemisphere = hemisphere==0 ? 1 : 0;
    uint32_t theRestMask = ~buildWord(std::numeric_limits<uint32_t>::max(), RUN3_CAND_SECTOR_ADDRESS_SHIFT, RUN3_CAND_SECTOR_ADDRESS_MASK);
    uint32_t theRest = maskedWord(word, 0u, theRestMask);
    return theRest |
           buildWord(sectorID, RUN3_CAND_SECTORID_SHIFT, RUN3_CAND_SECTORID_MASK) |
           buildWord(hemisphere, RUN3_SUBSYS_HEMISPHERE_SHIFT, RUN3_SUBSYS_HEMISPHERE_MASK);
  }

  /// Decode the data status word (returns a vector of bit indices for the errors set - empty if no errors)
  inline std::vector<size_t> getDataStatusWordErrors(uint32_t word) {
    uint16_t status = maskedWord(word, RUN3_STATUS_WORD_SHIFT, RUN3_STATUS_WORD_MASK);
    if (status==0) return {};
    std::vector<size_t> errors;
    for (size_t bit=0; bit<DataStatusWordErrors.size(); ++bit) {
      if (wordEquals(status, bit, 1u, 1u)) {
        errors.push_back(bit);
      }
    }
    return errors;
  }

  /// Encode the data status word
  inline constexpr uint32_t dataStatusWord(uint16_t status) {
    uint32_t word = buildWord(RUN3_STATUS_WORD_ID_VAL, RUN3_STATUS_WORD_ID_SHIFT, RUN3_STATUS_WORD_ID_MASK);
    word |= status;
    return word;
  }

} // namespace LVL1::MuCTPIBits

#endif // TRIGT1MUCTPIBITS_HELPERSPHASE1_H
