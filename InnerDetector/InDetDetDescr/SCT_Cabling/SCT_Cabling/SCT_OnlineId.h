/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_OnlineId_h
#define SCT_OnlineId_h
/**
 * @file SCT_OnlineId.h
 * Header file for a utility class to hold the online id
 **/

#include <cstdint>

/**
 * @class SCT_OnlineId
 * SCT_OnlineId is a class to hold an online id number and 
 * provide check on validity, and conversions between the different formats.
 * In general, an SCT online id has 32 bits
 * composed of the robId (0-23) and the fibre number (24-31)
 **/

class SCT_OnlineId{
public:

  /// Default constructor produces an invalid serial number
  SCT_OnlineId();
  
  /// Construct from uint32
  SCT_OnlineId(const std::uint32_t onlineId);
  
  /// Construct from robId and fibre; a cursory check is made on validity of the input
  SCT_OnlineId(const std::uint32_t rodId, const std::uint32_t fibre);
  
  /// Return the rod/rob Id
  std::uint32_t rod() const;
  
  /// Return the fibre
  std::uint32_t fibre() const;
  
  /// Overload cast to uint
  operator unsigned int() const;
  
  ///Check whether the onlineId is valid, with flag to switch between validity from the database or text file cabling source
  bool is_valid(const bool usingDbCabling) const;
  
  /// Check whether the onlineId is valid, without knowing the data source; this is a rough check
  bool is_valid() const;
  
  /// Return an index in the range 0-9215, calculated from parts of the onlineId
  unsigned int index() const;
  
  /// Is the rod in range?
  static bool rodIdInRange(std::uint32_t r);
  
  /// Constants for evaluating hash indices of the online id. The hashing formula is in 'index()'
  enum {
    FIRST_FIBRE=0, LAST_FIBRE=95, INVALID_FIBRE=255, INVALID_ROD=16777215, NUM_FIBRES=96, MAX_INDEX=19871, NUMBER_OF_INDICES=19872, INVALID_INDEX=0xFFFFFFFF, INVALID_ONLINE_ID=0xFFFFFFFF
  };
  
  /// Implement pre-increment and post-increment for iterating over fibres in a rod
  SCT_OnlineId &operator++();
  SCT_OnlineId operator++(int);
  
private:
  /// The underlying number
  std::uint32_t m_onlineId;
  
  /// Simple range check
  bool fibreInRange(std::uint32_t f) const;
  
  /// Rough check on validity
  bool couldBeValid(std::uint32_t r);
  
  /// Check on validity
  bool rodIdInRange(std::uint32_t r, const bool usingDbCabling) const;

};

#endif
