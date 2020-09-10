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
  ATH_CHECK(m_BSErrContReadKey.initialize(SG::AllowEmpty));

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  ATH_CHECK(m_condTDAQKey.initialize( !m_condTDAQKey.empty() ));
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
  if (not idcErrCont.isValid()) { return nullptr; }
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
  if (!m_useByteStream) { return 0; }

  std::lock_guard<std::mutex> lock{m_cacheMutex};
  auto idcCachePtr = getCacheEntry(ctx)->IDCCache;
  if (idcCachePtr==nullptr) {
    ATH_MSG_ERROR("PixelConditionsSummaryTool No cache! " );
  }
  uint64_t word = (uint64_t)idcCachePtr->retrieve(moduleHash);
  return word<m_missingErrorInfo ? word : 0;
}

bool PixelConditionsSummaryTool::hasBSError(const IdentifierHash& moduleHash) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return hasBSError(moduleHash, ctx);
}

bool PixelConditionsSummaryTool::hasBSError(const IdentifierHash& moduleHash, Identifier pixid) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return hasBSError(moduleHash, pixid, ctx);
}

bool PixelConditionsSummaryTool::hasBSError(const IdentifierHash& moduleHash, const EventContext& ctx) const {
  if (!m_useByteStream) { return false; }
  uint64_t word = getBSErrorWord(moduleHash,ctx);
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::TimeOut))         { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::BCID))            { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::LVL1ID))          { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Preamble))        { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Trailer))         { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Decoding))        { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Invalid))         { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::LinkMaskedByPPC)) { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Limit))           { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::TruncatedROB))    { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MaskedROB))       { return true; }

  return false;
}

bool PixelConditionsSummaryTool::hasBSError(const IdentifierHash& moduleHash, Identifier pixid, const EventContext& ctx) const {
  if (!m_useByteStream) { return false; }

  if (hasBSError(moduleHash, ctx)) { return true; }

  int maxHash = m_pixelID->wafer_hash_max();
  Identifier moduleID = m_pixelID->wafer_id(pixid);
  int chFE = m_pixelCabling->getFE(&pixid,moduleID);

  int indexFE = (1+chFE)*maxHash+(int)moduleHash;    // (FE_channel+1)*2048 + moduleHash
  uint64_t word = getBSErrorWord(indexFE,ctx);
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::TimeOut))         { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::BCID))            { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::LVL1ID))          { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Preamble))        { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Trailer))         { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Decoding))        { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Invalid))         { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::LinkMaskedByPPC)) { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Limit))           { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::TruncatedROB))    { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MaskedROB))       { return true; }

  return false;
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

  if (m_useByteStream && hasBSError(moduleHash, ctx)) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey,ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  if (!m_condTDAQKey.empty() && SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash& moduleHash, const Identifier& elementId, const EventContext& ctx) const {

  if (m_useByteStream && hasBSError(moduleHash, ctx)) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey,ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  if (!m_condTDAQKey.empty() && SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  return checkChipStatus(moduleHash, elementId);
}

double PixelConditionsSummaryTool::activeFraction(const IdentifierHash & /*moduleHash*/, const Identifier & /*idStart*/, const Identifier & /*idEnd*/) const {
  return 1.0;
}

double PixelConditionsSummaryTool::activeFraction(const IdentifierHash & /*moduleHash*/, const Identifier & /*idStart*/, const Identifier & /*idEnd*/, const EventContext& /*ctx*/) const {
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

  if (!m_condTDAQKey.empty() && SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  if (h==InDetConditions::PIXEL_CHIP) {
    if (!checkChipStatus(moduleHash, elementId)) { return false; }
    if (m_useByteStream && hasBSError(moduleHash, elementId, ctx)) { return false; }
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

  if (!m_condTDAQKey.empty() && SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

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

  if (!m_condTDAQKey.empty() && SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelModuleData>(m_condDeadMapKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  if (!checkChipStatus(moduleHash, elementId)) { return false; }

  if (m_useByteStream && hasBSError(moduleHash, elementId, ctx)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::checkChipStatus(IdentifierHash moduleHash, Identifier pixid) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return checkChipStatus(moduleHash, pixid, ctx);
}

double PixelConditionsSummaryTool::goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return goodFraction(moduleHash, idStart, idEnd, ctx);
}

double PixelConditionsSummaryTool::goodFraction(const IdentifierHash & moduleHash, const Identifier & idStart, const Identifier & idEnd, const EventContext& ctx) const {

  if (!isGood(moduleHash, ctx)) { return 0.0; }

  Identifier moduleID = m_pixelID->wafer_id(moduleHash);

  int phiStart = m_pixelID->phi_index(idStart);
  int etaStart = m_pixelID->eta_index(idStart);

  int phiEnd = m_pixelID->phi_index(idEnd);
  int etaEnd = m_pixelID->eta_index(idEnd);

  double nTotal = (std::abs(phiStart-phiEnd)+1.0)*(std::abs(etaStart-etaEnd)+1.0);

  double nGood = 0.0;
  for (int i=std::min(phiStart,phiEnd); i<=std::max(phiStart,phiEnd); i++) {
    for (int j=std::min(etaStart,etaEnd); j<=std::max(etaStart,etaEnd); j++) {
      if (checkChipStatus(moduleHash, m_pixelID->pixel_id(moduleID,i,j), ctx)) { 
        if (m_useByteStream) {
          if (!hasBSError(moduleHash, m_pixelID->pixel_id(moduleID,i,j), ctx)) { nGood++; }
        }
        else {
          nGood++; 
        }
      }
    }
  }
  return nGood/nTotal;
}

