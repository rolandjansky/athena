/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
//uint32 in std: namespace
#include <cstdint>

// Athena includes
#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/SCT_OnlineId.h"
#include "SCT_Cabling/SCT_SerialNumber.h"

class SCT_CablingData {
 public:
  SCT_CablingData(); // constructor
  ~SCT_CablingData() = default; // destructor

  void invalidate();

  void invalidateHashesForOnlineIds();
  bool setHashForOnlineId(const IdentifierHash& hash, const SCT_OnlineId& onlineId);
  IdentifierHash getHashFromOnlineId(const SCT_OnlineId& onlineId) const;

  void invalidateOnlineIdsForHashes();
  bool setOnlineIdForHash(const SCT_OnlineId& onlineId, const IdentifierHash& hash);
  SCT_OnlineId getOnlineIdFromHash(const IdentifierHash& hash) const;

  void invalidateHashesForSerialNumbers();
  bool setHashForSerialNumber(const IdentifierHash& hash, const SCT_SerialNumber& sn);
  IdentifierHash getHashFromSerialNumber(const SCT_SerialNumber& sn) const;

  void invalidateSerialNumbersForHashes();
  bool setSerialNumberForHash(const SCT_SerialNumber& sn, const IdentifierHash& hash);
  SCT_SerialNumber getSerialNumberFromHash(const IdentifierHash& hash) const;

  void invalidateRods();
  void setRod(const std::uint32_t& rod);
  void getRods(std::vector<std::uint32_t>& usersVector) const;

  unsigned int getHashEntries() const;

  enum {NUMBER_OF_HASHES=8176}; // In SCT, we have 8176 wafers.

 private:
  std::array<IdentifierHash, SCT_OnlineId::NUMBER_OF_INDICES> m_onlineId2HashArray;
  std::array<SCT_OnlineId, NUMBER_OF_HASHES> m_hash2OnlineIdArray; //!<Array for hash to onlineId; hash goes from 0-8175
  std::map<SCT_SerialNumber, IdentifierHash> m_sn2HashMap; //!<Data map for serial number to hash
  std::array<SCT_SerialNumber, NUMBER_OF_HASHES> m_hash2SnArray; //!<Array for hash to serial number
  std::set<std::uint32_t> m_rodIdSet; //!<set of robIds

  static const IdentifierHash s_invalidHash;
  static const SCT_OnlineId s_invalidId;
  static const SCT_SerialNumber s_invalidSn;
};

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_CablingData , 42909094 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_CablingData , 160983208 );

#endif // SCT_CablingData_h
