/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CablingTool.cxx
 * Implementation file for SCT cabling service
 * @author Shaun Roe
 * @author Susumu Oda
 * @date 19 August, 2018
 **/
 
//this package
#include "SCT_CablingTool.h"

//Athena
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "StoreGate/ReadCondHandle.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//constants in file scope
static const std::string coracool("CORACOOL");
static const std::string coolVectorPayload("COOLVECTOR");
static const std::string defaultSource(coracool);
static const std::string file("SCT_Sept08Cabling_svc.dat");
//invalid identifiers to return in case of error
static const SCT_OnlineId invalidId;
static const IdentifierHash invalidHash;
static const SCT_SerialNumber invalidSn;

//Utility functions at file scope
namespace {
  //make a number even
  IdentifierHash even(const IdentifierHash& hash) {
    return (hash>>1) << 1;
  }
}

// Constructor
SCT_CablingTool::SCT_CablingTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_idHelper{nullptr},
  m_usingDatabase{true}
{
  declareProperty("DataSource", m_cablingDataSource=defaultSource);
}

//
StatusCode
SCT_CablingTool::initialize() {
  ATH_MSG_INFO("Initialize SCT cabling");
  const std::string cablingDataSource = m_cablingDataSource.value();
  m_usingDatabase=(cablingDataSource == coracool) or (cablingDataSource == coolVectorPayload) or (cablingDataSource == file);
  ATH_CHECK(detStore()->retrieve(m_idHelper, "SCT_ID"));
  ATH_CHECK(m_data.initialize());
  return StatusCode::SUCCESS;
}

// 
StatusCode
SCT_CablingTool::finalize() {
  ATH_MSG_INFO("Thank-you for using the SCT_CablingTool");
  return StatusCode::SUCCESS;
}

//
unsigned int
SCT_CablingTool::size(const EventContext& ctx) const {
  const SCT_CablingData* data{getData(ctx)};
  if (data==nullptr) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return 0;
  }

  return data->getHashEntries();
}

unsigned int
SCT_CablingTool::size() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return size(ctx);
}

//
bool
SCT_CablingTool::empty(const EventContext& ctx) const {
  return (size(ctx)==0);
}

bool
SCT_CablingTool::empty() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return empty(ctx);
}

//
IdentifierHash 
SCT_CablingTool::getHashFromOnlineId(const SCT_OnlineId& onlineId, const EventContext& ctx, const bool withWarnings) const {
  //is it valid at all?
  if (not onlineId.is_valid()) {
    if (withWarnings) ATH_MSG_WARNING("Invalid online id ("<<std::hex<<onlineId<<") "<<std::dec);
    return invalidHash;
  }
  //is it specifically valid for the given datasource?
  if (not onlineId.is_valid(m_usingDatabase)) {
    const std::string alternative=m_usingDatabase?"text file cabling":"cabling from database";
    if (withWarnings) ATH_MSG_WARNING("Invalid online id ("<<std::hex<<onlineId<<") try using the "<<alternative<<std::dec);
    return invalidHash;
  }

  const SCT_CablingData* data{getData(ctx)};
  if (data==nullptr) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidHash;
  }

  return data->getHashFromOnlineId(onlineId);
}

IdentifierHash
SCT_CablingTool::getHashFromOnlineId(const SCT_OnlineId& onlineId, const bool withWarnings) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getHashFromOnlineId(onlineId, ctx, withWarnings);
}

//
SCT_OnlineId 
SCT_CablingTool::getOnlineIdFromHash(const IdentifierHash& hash, const EventContext& ctx) const {
  const SCT_CablingData* data{getData(ctx)};
  if (data==nullptr) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidId;
  }
  
  return data->getOnlineIdFromHash(hash);
}

SCT_OnlineId
SCT_CablingTool::getOnlineIdFromHash(const IdentifierHash& hash) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getOnlineIdFromHash(hash, ctx);
}

//
SCT_OnlineId
SCT_CablingTool::getOnlineIdFromOfflineId(const Identifier& offlineId, const EventContext& ctx) const {
  if (not offlineId.is_valid()) return invalidId;
  IdentifierHash hash(m_idHelper->wafer_hash(offlineId));
  return getOnlineIdFromHash(hash, ctx);
}

SCT_OnlineId
SCT_CablingTool::getOnlineIdFromOfflineId(const Identifier& offlineId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getOnlineIdFromOfflineId(offlineId, ctx);
}

//
std::uint32_t
SCT_CablingTool::getRobIdFromHash(const IdentifierHash& hash, const EventContext& ctx) const {
  return getOnlineIdFromHash(hash, ctx).rod();
}

std::uint32_t
SCT_CablingTool::getRobIdFromHash(const IdentifierHash& hash) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getRobIdFromHash(hash, ctx);
}

//
std::uint32_t
SCT_CablingTool::getRobIdFromOfflineId(const Identifier& offlineId, const EventContext& ctx) const {
  return getOnlineIdFromOfflineId(offlineId, ctx).rod();
}

std::uint32_t
SCT_CablingTool::getRobIdFromOfflineId(const Identifier& offlineId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getRobIdFromOfflineId(offlineId, ctx);
}

//
IdentifierHash
SCT_CablingTool::getHashFromSerialNumber(const SCT_SerialNumber& sn, const EventContext& ctx) const {
  if (not sn.isWellFormed()) return invalidHash;

  const SCT_CablingData* data{getData(ctx)};
  if (data==nullptr) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidHash;
  }

  return data->getHashFromSerialNumber(sn);
}

IdentifierHash
SCT_CablingTool::getHashFromSerialNumber(const SCT_SerialNumber& sn) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getHashFromSerialNumber(sn, ctx);
}

SCT_SerialNumber
SCT_CablingTool::getSerialNumberFromHash(const IdentifierHash& hash, const EventContext& ctx) const {
  if (not hash.is_valid()) return invalidSn;
  //hash must be even
  IdentifierHash evenHash{even(hash)};
  
  const SCT_CablingData* data{getData(ctx)};
  if (data==nullptr) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return invalidSn;
  }

  return data->getSerialNumberFromHash(evenHash);
}

SCT_SerialNumber
SCT_CablingTool::getSerialNumberFromHash(const IdentifierHash& hash) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getSerialNumberFromHash(hash, ctx);
}

void
SCT_CablingTool::getAllRods(std::vector<std::uint32_t>& usersVector, const EventContext& ctx) const {
  const SCT_CablingData* data{getData(ctx)};
  if (data==nullptr) {
    ATH_MSG_FATAL("Filling the cabling FAILED");
    return;
  }

  data->getRods(usersVector);
}

void
SCT_CablingTool::getAllRods(std::vector<std::uint32_t>& usersVector) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  getAllRods(usersVector, ctx);
}

void
SCT_CablingTool::getHashesForRod(std::vector<IdentifierHash>& usersVector, const std::uint32_t rodId, const EventContext& ctx) const {
  SCT_OnlineId firstPossibleId{rodId, SCT_OnlineId::FIRST_FIBRE};
  const bool withWarnings{false};
  for (SCT_OnlineId i{firstPossibleId}; i!=SCT_OnlineId::INVALID_ONLINE_ID; ++i) {
    IdentifierHash thisHash(getHashFromOnlineId(i, ctx, withWarnings));
    if (thisHash != invalidHash) {
      usersVector.push_back(thisHash);
    }
  }
}

void
SCT_CablingTool::getHashesForRod(std::vector<IdentifierHash>& usersVector, const std::uint32_t rodId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  getHashesForRod(usersVector, rodId, ctx);
}

const SCT_CablingData*
SCT_CablingTool::getData(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_CablingData> condData{m_data, ctx};
  return condData.retrieve();
}
