/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsSummaryTool.h"

PixelConditionsSummaryTool::PixelConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent)
  :AthAlgTool(type, name, parent),
  m_pixelID(nullptr),
  m_useByteStream(false)
{
  m_isActiveStatus.push_back("OK");
  m_isActiveStates.push_back("READY");

  declareProperty("IsActiveStatus", m_isActiveStatus);
  declareProperty("IsActiveStates", m_isActiveStates);
  declareProperty("UseByteStream", m_useByteStream, "Switch for usage of the ByteStream error service");
}

PixelConditionsSummaryTool::~PixelConditionsSummaryTool(){}

StatusCode PixelConditionsSummaryTool::initialize(){
  ATH_MSG_DEBUG("PixelConditionsSummaryTool::initialize()");

  ATH_CHECK(setProperties());

  ATH_CHECK(m_condDCSStateKey.initialize());
  ATH_CHECK(m_condDCSStatusKey.initialize());
  ATH_CHECK(m_BSErrContReadKey.initialize(m_useByteStream && !m_BSErrContReadKey.empty()));

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  ATH_CHECK(m_condTDAQKey.initialize());
  ATH_CHECK(m_condDeadMapKey.initialize());
  ATH_CHECK(m_pixelCabling.retrieve());

  for (unsigned int istate=0; istate<m_isActiveStates.size(); istate++) {
    if      (m_isActiveStates[istate]=="READY")      { m_activeState.push_back(PixelDCSStateData::DCSModuleState::READY); }
    else if (m_isActiveStates[istate]=="ON")         { m_activeState.push_back(PixelDCSStateData::DCSModuleState::ON); }
    else if (m_isActiveStates[istate]=="UNKNOWN")    { m_activeState.push_back(PixelDCSStateData::DCSModuleState::UNKNOWN); }
    else if (m_isActiveStates[istate]=="TRANSITION") { m_activeState.push_back(PixelDCSStateData::DCSModuleState::TRANSITION); }
    else if (m_isActiveStates[istate]=="UNDEFINED")  { m_activeState.push_back(PixelDCSStateData::DCSModuleState::UNDEFINED); }
    else if (m_isActiveStates[istate]=="NOSTATE")    { m_activeState.push_back(PixelDCSStateData::DCSModuleState::NOSTATE); }
    else {
      ATH_MSG_ERROR("No matching DCS state " << m_isActiveStates[istate] << " in DCSModuleState");
      return StatusCode::FAILURE;
    }
  }

  for (unsigned int istatus=0; istatus<m_isActiveStatus.size(); istatus++) {
    if      (m_isActiveStatus[istatus]=="OK")       { m_activeStatus.push_back(PixelDCSStatusData::DCSModuleStatus::OK); }
    else if (m_isActiveStatus[istatus]=="WARNING")  { m_activeStatus.push_back(PixelDCSStatusData::DCSModuleStatus::WARNING); }
    else if (m_isActiveStatus[istatus]=="ERROR")    { m_activeStatus.push_back(PixelDCSStatusData::DCSModuleStatus::ERROR); }
    else if (m_isActiveStatus[istatus]=="FATAL")    { m_activeStatus.push_back(PixelDCSStatusData::DCSModuleStatus::FATAL); }
    else if (m_isActiveStatus[istatus]=="NOSTATUS") { m_activeStatus.push_back(PixelDCSStatusData::DCSModuleStatus::NOSTATUS); }
    else {
      ATH_MSG_ERROR("No matching DCS status " << m_isActiveStatus[istatus] << " in DCSModuleStatus");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

const IDCInDetBSErrContainer* PixelConditionsSummaryTool::getContainer(const EventContext& ctx) const {
  SG::ReadHandle<IDCInDetBSErrContainer> idcErrCont(m_BSErrContReadKey, ctx);
  /** When running over ESD files without BSErr container stored, don't
   * want to flood the user with error messages. Should just have a bunch
   * of empty sets, and keep quiet.
   */
  if (not idcErrCont.isValid()) {
    m_nRetrievalFailure++;
    if (m_nRetrievalFailure<=3) {
      ATH_MSG_INFO("PixelConditionsSummaryTool Failed to retrieve BS error container " << m_BSErrContReadKey.key() << " from StoreGate.");
      if (m_nRetrievalFailure==3) {
        ATH_MSG_INFO("PixelConditionsSummaryTool This message on retrieval failure of " << m_BSErrContReadKey.key() << " is suppressed.");
      }
    }
    return nullptr;
  }
  ATH_MSG_VERBOSE("PixelConditionsSummaryTool IDC Container fetched " << m_BSErrContReadKey.key());
  return idcErrCont.cptr();
}

PixelConditionsSummaryTool::IDCCacheEntry* PixelConditionsSummaryTool::getCacheEntry(const EventContext& ctx) const {
  IDCCacheEntry* cacheEntry = m_eventCache.get(ctx);
  if (cacheEntry->needsUpdate(ctx)) {
    auto idcErrContPtr = getContainer(ctx);
    if (idcErrContPtr==nullptr) {     // missing or not, the cache needs to be reset
      cacheEntry->reset(ctx.evt(), nullptr);
    }
    else {
      cacheEntry->reset(ctx.evt(), idcErrContPtr->cache());
    }
    ATH_MSG_VERBOSE("PixelConditionsSummaryTool Cache for the event reset " << cacheEntry->eventId << " with IDC container" << idcErrContPtr);
  }
  return cacheEntry;
}

uint64_t PixelConditionsSummaryTool::getBSErrorWord(const IdentifierHash& moduleHash, const EventContext& ctx) const {
  std::lock_guard<std::mutex> lock{m_cacheMutex};
  auto idcCachePtr = getCacheEntry(ctx)->IDCCache;
  if (idcCachePtr==nullptr) {
    ATH_MSG_ERROR("PixelConditionsSummaryTool No cache! " );
  }
  return (uint64_t)idcCachePtr->retrieve(moduleHash);
}

bool PixelConditionsSummaryTool::hasBSError(const IdentifierHash& moduleHash, const EventContext& ctx) const {
  if (!m_useByteStream) { return false; }

  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::TimeOut))         { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::BCID))            { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::LVL1ID))          { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::Preamble))        { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::Trailer))         { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::Decoding))        { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::Invalid))         { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::LinkMaskedByPPC)) { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::Limit))           { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::TruncatedROB))    { return true; }
  if (PixelByteStreamErrors::hasError(getBSErrorWord(moduleHash,ctx),PixelByteStreamErrors::MaskedROB))       { return true; }

// STSTST  if (m_pixelID->wafer_hash_max()==2048) {   // RUN-2 setup
// STSTST    if ((m_pixelID->barrel_ec(m_pixelID->wafer_id(moduleHash))==0 && m_pixelID->layer_disk(m_pixelID->wafer_id(moduleHash))==0) 
// STSTST        || m_pixelID->is_dbm(m_pixelID->wafer_id(moduleHash))) {
// STSTST      for (const auto* elt : *errCont) {
// STSTST        IdentifierHash myHash=elt->first;
// STSTST        if (myHash-m_pixelID->wafer_hash_max()==moduleHash) { return false; }
// STSTST      }
// STSTST      return true;
// STSTST    }
// STSTST  }
// STSTST
// STSTST  int errorcode = 0;
// STSTST  for (const auto* elt : *errCont) {
// STSTST    IdentifierHash myHash=elt->first;
// STSTST    if (myHash==moduleHash) { errorcode = elt->second; break; }
// STSTST  }
// STSTST  if ((errorcode & 0xFFF1F00F) == 0) { // Mask FE errors
// STSTST    for (const auto* elt : *errCont) {
// STSTST      IdentifierHash myHash=elt->first;
// STSTST      if (myHash-m_pixelID->wafer_hash_max()==moduleHash) { return false; }
// STSTST    }
// STSTST    return true;
// STSTST  }
// STSTST  else if (errorcode) {
// STSTST    return false;
// STSTST  }
  return false;
}

bool PixelConditionsSummaryTool::isBSActive(const IdentifierHash & moduleHash, const EventContext& ctx) const {
  if (m_useByteStream && hasBSError(moduleHash, ctx)) { return false; }
  return true;
}

bool PixelConditionsSummaryTool::isActive(const Identifier& elementId, const InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isActive(elementId, h, ctx);
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash& moduleHash) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isActive(moduleHash, ctx);
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash& moduleHash, const Identifier& elementId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isActive(moduleHash, elementId, ctx);
}

bool PixelConditionsSummaryTool::isActive(const Identifier& /*elementId*/, const InDetConditions::Hierarchy /*h*/, const EventContext& /*ctx*/) const {
  return true;
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash& moduleHash, const EventContext& ctx) const {

  if (m_useByteStream && !isBSActive(moduleHash,ctx)) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey,ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  if (SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash& moduleHash, const Identifier& elementId, const EventContext& ctx) const {

  if (m_useByteStream && !isBSActive(moduleHash,ctx)) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey,ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  if (SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  return checkChipStatus(moduleHash, elementId);
}

double PixelConditionsSummaryTool::activeFraction(const IdentifierHash & /*moduleHash*/, const Identifier & /*idStart*/, const Identifier & /*idEnd*/) const {
  return 1.0;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash & moduleHash) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(moduleHash, ctx);
}

bool PixelConditionsSummaryTool::isGood(const Identifier& elementId, const InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(elementId, h, ctx);
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash & moduleHash, const Identifier &elementId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(moduleHash, elementId, ctx);
}

bool PixelConditionsSummaryTool::isGood(const Identifier& elementId, const InDetConditions::Hierarchy h, const EventContext& ctx) const {
  Identifier moduleID       = m_pixelID->wafer_id(elementId);
  IdentifierHash moduleHash = m_pixelID->wafer_hash(moduleID);

  if (m_useByteStream && hasBSError(moduleHash, ctx)) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey, ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  SG::ReadCondHandle<PixelDCSStatusData> dcsstatus_data(m_condDCSStatusKey, ctx);
  bool isDCSGood = false;
  for (unsigned int istatus=0; istatus<m_activeStatus.size(); istatus++) {
    if (m_activeStatus[istatus]==dcsstatus_data->getModuleStatus(moduleHash)) { isDCSGood=true; }
  }
  if (!isDCSGood) { return false; }

  if (SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  if (h==InDetConditions::PIXEL_CHIP) {
    return checkChipStatus(moduleHash, elementId);
  }

  return true;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash& moduleHash, const EventContext& ctx) const {

  if (m_useByteStream && hasBSError(moduleHash, ctx)) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey, ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  SG::ReadCondHandle<PixelDCSStatusData> dcsstatus_data(m_condDCSStatusKey, ctx);
  bool isDCSGood = false;
  for (unsigned int istatus=0; istatus<m_activeStatus.size(); istatus++) {
    if (m_activeStatus[istatus]==dcsstatus_data->getModuleStatus(moduleHash)) { isDCSGood=true; }
  }
  if (!isDCSGood) { return false; }

  if (SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash & moduleHash, const Identifier &elementId, const EventContext& ctx) const {

  if (m_useByteStream && hasBSError(moduleHash, ctx)) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey, ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  SG::ReadCondHandle<PixelDCSStatusData> dcsstatus_data(m_condDCSStatusKey, ctx);
  bool isDCSGood = false;
  for (unsigned int istatus=0; istatus<m_activeStatus.size(); istatus++) {
    if (m_activeStatus[istatus]==dcsstatus_data->getModuleStatus(moduleHash)) { isDCSGood=true; }
  }
  if (!isDCSGood) { return false; }

  if (SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  return checkChipStatus(moduleHash, elementId);
}

double PixelConditionsSummaryTool::goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const {

  if (!isGood(moduleHash)) { return 0.0; }

  Identifier moduleID = m_pixelID->wafer_id(moduleHash);

  int phiStart = m_pixelID->phi_index(idStart);
  int etaStart = m_pixelID->eta_index(idStart);

  int phiEnd = m_pixelID->phi_index(idEnd);
  int etaEnd = m_pixelID->eta_index(idEnd);

  double nTotal = (std::abs(phiStart-phiEnd)+1.0)*(std::abs(etaStart-etaEnd)+1.0);

  double nGood = 0.0;
  for (int i=std::min(phiStart,phiEnd); i<=std::max(phiStart,phiEnd); i++) {
    for (int j=std::min(etaStart,etaEnd); j<=std::max(etaStart,etaEnd); j++) {
      if (checkChipStatus(moduleHash, m_pixelID->pixel_id(moduleID,i,j))) { nGood++; }
    }
  }
  return nGood/nTotal;
}

