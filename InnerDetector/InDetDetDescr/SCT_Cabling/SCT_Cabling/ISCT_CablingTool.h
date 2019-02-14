/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISCT_CablingTool_h
#define ISCT_CablingTool_h
/**
 * @file ISCT_CablingTool.h
 * Header file for abstract interface to SCT cabling tool
 * @author Shaun Roe
 * @author Susumu Oda
 * @date 19 August, 2018
 **/

//InnerDetector
//to ensure clients can actually use the conversion constructors, include these here (otherwise could be forward declared)
#include "SCT_Cabling/SCT_SerialNumber.h"
#include "SCT_Cabling/SCT_OnlineId.h"

//Gaudi includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"

//standard includes
#include <cstdint> //C++11
#include <vector>

//fwd declarations
class Identifier;
class IdentifierHash;

/**
 * @class ISCT_CablingTool
 * Client interface to the cabling, providing conversions between online and offline identifiers
 **/
class ISCT_CablingTool: virtual public IAlgTool {
 public:
  /// no-op destructor
  virtual ~ISCT_CablingTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_CablingTool, 1, 0);

  /// return offline hash, given the online Id (used by decoders)
  virtual IdentifierHash getHashFromOnlineId(const SCT_OnlineId& onlineId, const bool withWarnings = true) const = 0;
  virtual IdentifierHash getHashFromOnlineId(const SCT_OnlineId& onlineId, const EventContext& ctx, const bool withWarnings = true) const = 0;

  /// return the online Id, given a hash (used by simulation encoders)
  virtual SCT_OnlineId getOnlineIdFromHash(const IdentifierHash& hash) const = 0;
  virtual SCT_OnlineId getOnlineIdFromHash(const IdentifierHash& hash, const EventContext& ctx) const = 0;

  /// return the rob/rod Id, given a hash (used by simulation encoders)
  virtual std::uint32_t getRobIdFromHash(const IdentifierHash& hash) const = 0;
  virtual std::uint32_t getRobIdFromHash(const IdentifierHash& hash, const EventContext& ctx) const = 0;

  /// return the online Id, given an offlineId
  virtual SCT_OnlineId getOnlineIdFromOfflineId(const Identifier& offlineId) const = 0;
  virtual SCT_OnlineId getOnlineIdFromOfflineId(const Identifier& offlineId, const EventContext& ctx) const = 0;

  /// return the rob/rod Id, given an offlineId (used by simulation encoders)
  virtual std::uint32_t getRobIdFromOfflineId(const Identifier& offlineId) const = 0;
  virtual std::uint32_t getRobIdFromOfflineId(const Identifier& offlineId, const EventContext& ctx) const = 0;

  /// size of the data structure (for the SCT should be 8176, one for each module side)
  virtual unsigned int size() const = 0;
  virtual unsigned int size(const EventContext& ctx) const = 0;

  /// is the data structure empty?
  virtual bool empty() const = 0;
  virtual bool empty(const EventContext& ctx) const = 0;

  /// get hash from a module serial number, needed in the conditions service because configurations are stored by module s/n
  virtual IdentifierHash getHashFromSerialNumber(const SCT_SerialNumber& sn) const = 0;
  virtual IdentifierHash getHashFromSerialNumber(const SCT_SerialNumber& sn, const EventContext& ctx) const = 0;

  /// get module serial number from hash, needed during filling of data structure
  virtual SCT_SerialNumber getSerialNumberFromHash(const IdentifierHash& hash) const = 0;
  virtual SCT_SerialNumber getSerialNumberFromHash(const IdentifierHash& hash, const EventContext& ctx) const = 0;

  /// fill a users vector with all the RodIds
  virtual void getAllRods(std::vector<std::uint32_t>& usersVector) const = 0;
  virtual void getAllRods(std::vector<std::uint32_t>& usersVector, const EventContext& ctx) const = 0;

  /// fill a user's vector with all the hash ids which belong to a given rod
  virtual void getHashesForRod(std::vector<IdentifierHash>& usersVector, const std::uint32_t rodId) const = 0;
  virtual void getHashesForRod(std::vector<IdentifierHash>& usersVector, const std::uint32_t rodId, const EventContext& ctx) const = 0;
};

#endif // ISCT_CablingTool_h
