/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CablingData_h
#define SCT_CablingData_h

/**
 * @file SCT_CablingSvc.h
 * Header file for  SCT cabling data
 * @author Susumu Oda
 * @date 7 March, 2018
 **/

// STL includes
#include <array>
#include <map>
// uint32 in std namespace
#include <cstdint>

// Athena includes
#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/SCT_OnlineId.h"
#include "SCT_Cabling/SCT_SerialNumber.h"

/**
 * @class SCT_CablingData
 * A class to hold the data necessary for SCT_CablingTool.
 * This class is filled by
 * SCT_CablingCondAlgFromCoraCool or SCT_CablingCondAlgFromText for SCT_CablingTool.
 **/

class SCT_CablingData {
 public:
  /// constructor
  SCT_CablingData();
  /// destructor
  ~SCT_CablingData() = default;

  /// Default copy constructor
  SCT_CablingData(const SCT_CablingData&) = default;
  /// Default assignment operator
  SCT_CablingData& operator=(const SCT_CablingData&) = default;
  /// Default move constructor
  SCT_CablingData(SCT_CablingData&&) = default;
  /// Default move assignment operator
  SCT_CablingData& operator=(SCT_CablingData&&) = default;

  /// Invalidate all data members
  void invalidate();

  /// Invalidate m_onlineId2HashArray
  void invalidateHashesForOnlineIds();
  /// Set m_onlineId2HashArray
  bool setHashForOnlineId(const IdentifierHash& hash, const SCT_OnlineId& onlineId);
  /// Get IdentifierHash from SCT_OnlineId
  IdentifierHash getHashFromOnlineId(const SCT_OnlineId& onlineId) const;

  /// Invalidate m_hash2OnlineIdArray
  void invalidateOnlineIdsForHashes();
  /// Set m_hash2OnlineIdArray
  bool setOnlineIdForHash(const SCT_OnlineId& onlineId, const IdentifierHash& hash);
  /// Get SCT_OnlineId from IdentifierHash
  SCT_OnlineId getOnlineIdFromHash(const IdentifierHash& hash) const;

  /// Invalidate m_sn2HashMap
  void invalidateHashesForSerialNumbers();
  /// Set m_sn2HashMap
  bool setHashForSerialNumber(const IdentifierHash& hash, const SCT_SerialNumber& sn);
  /// Get IdentifierHash from SCT_SerialNumber
  IdentifierHash getHashFromSerialNumber(const SCT_SerialNumber& sn) const;

  /// Invalidate m_hash2SnArray
  void invalidateSerialNumbersForHashes();
  /// Set m_hash2SnArray
  bool setSerialNumberForHash(const SCT_SerialNumber& sn, const IdentifierHash& hash);
  /// Get SCT_SerialNumber from IdentifierHash
  SCT_SerialNumber getSerialNumberFromHash(const IdentifierHash& hash) const;

  /// Invalidate m_rodIdSet
  void invalidateRods();
  /// Set m_rodIdSet
  void setRod(const std::uint32_t& rod);
  /// Get a vector of all RODs
  void getRods(std::vector<std::uint32_t>& usersVector) const;

  /// Return the size of m_sn2HashMap
  unsigned int getHashEntries() const;

  enum {NUMBER_OF_HASHES=8176}; // In SCT, we have 8176 wafers.

 private:
  std::array<IdentifierHash, SCT_OnlineId::NUMBER_OF_INDICES> m_onlineId2HashArray; //!< Array for onlineId to hash
  std::array<SCT_OnlineId, NUMBER_OF_HASHES> m_hash2OnlineIdArray; //!< Array for hash to onlineId; hash goes from 0-8175
  std::map<SCT_SerialNumber, IdentifierHash> m_sn2HashMap; //!< Data map for serial number to hash
  std::array<SCT_SerialNumber, NUMBER_OF_HASHES> m_hash2SnArray; //!< Array for hash to serial number
  std::set<std::uint32_t> m_rodIdSet; //!< Set of robIds

  static const IdentifierHash s_invalidHash; //!< Invalid IdentifierHash
  static const SCT_OnlineId s_invalidId; //!< Invalid SCT_OnlineId
  static const SCT_SerialNumber s_invalidSn; //!< Invalid SCT_SerialNumber
};

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_CablingData , 42909094 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_CablingData , 160983208 );

#endif // SCT_CablingData_h
