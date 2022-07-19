/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsSummaryTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "PixelDetectorElementStatus.h"
#include "PixelReadoutGeometry/PixelFEUtils.h"
#include "PixelConditionsData/PixelByteStreamErrors.h"

PixelConditionsSummaryTool::PixelConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent)
  :AthAlgTool(type, name, parent),
  m_pixelID(nullptr)
{
  m_isActiveStatus.emplace_back("OK");
  m_isActiveStates.emplace_back("READY");

  declareProperty("IsActiveStatus", m_isActiveStatus);
  declareProperty("IsActiveStates", m_isActiveStates);
}

PixelConditionsSummaryTool::~PixelConditionsSummaryTool(){}

StatusCode PixelConditionsSummaryTool::initialize(){
  ATH_MSG_DEBUG("PixelConditionsSummaryTool::initialize()");

  ATH_CHECK(m_condDCSStateKey.initialize());
  ATH_CHECK(m_condDCSStatusKey.initialize());

  bool useByteStream = (m_useByteStreamFEI4 || m_useByteStreamFEI3 || m_useByteStreamRD53);
  ATH_CHECK(m_BSErrContReadKey.initialize(useByteStream && !m_BSErrContReadKey.empty()));
  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));
  ATH_CHECK(m_condTDAQKey.initialize( !m_condTDAQKey.empty() ));
  ATH_CHECK(m_condDeadMapKey.initialize());
  ATH_CHECK(m_pixelReadout.retrieve());
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  if (!m_pixelDetElStatusEventKey.empty() && !m_pixelDetElStatusCondKey.empty()) {
     ATH_MSG_FATAL("The event data (PixelDetElStatusEventDataBaseKey) and cond data (PixelDetElStatusCondDataBaseKey) keys cannot be set at the same time.");
  }
  ATH_CHECK(m_pixelDetElStatusEventKey.initialize( !m_pixelDetElStatusEventKey.empty()));
  ATH_CHECK(m_pixelDetElStatusCondKey.initialize( !m_pixelDetElStatusCondKey.empty()));

  m_activeStateMask=0;
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
    if (m_activeState.back()<0 || m_activeState.back()>31) {
      ATH_MSG_FATAL("Logic error: state id too large. Cannot be represented by a bit");
      return StatusCode::FAILURE;
    }
    m_activeStateMask |= (1u<<m_activeState.back());
  }

  m_activeStatusMask=0;
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
    if (m_activeStatus.back()<0 || m_activeStatus.back()>31) {
      ATH_MSG_FATAL("Logic error: status id too large. Cannot be represented by a bit");
      return StatusCode::FAILURE;
    }
    m_activeStatusMask |= (1u<<m_activeStatus.back());
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
    const auto *idcErrContPtr = getContainer(ctx);
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
  return getBSErrorWord(moduleHash, moduleHash, ctx);
}

uint64_t PixelConditionsSummaryTool::getBSErrorWord(const IdentifierHash& moduleHash, const int index, const EventContext& ctx) const {

  if (moduleHash>=m_pixelID->wafer_hash_max()) {
    ATH_MSG_WARNING("invalid moduleHash : " << moduleHash << " exceed maximum hash id: " << m_pixelID->wafer_hash_max());
    return 0;
  }

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey, ctx);
  const InDetDD::SiDetectorElementCollection* elements(*pixelDetEleHandle);
  if (not pixelDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_WARNING(m_pixelDetEleCollKey.fullKey() << " is not available.");
    return 0;
  }
  const InDetDD::SiDetectorElement *element = elements->getDetectorElement(moduleHash);
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  if (!m_useByteStreamFEI4 && p_design->getReadoutTechnology() == InDetDD::PixelReadoutTechnology::FEI4) { return 0; }
  if (!m_useByteStreamFEI3 && p_design->getReadoutTechnology() == InDetDD::PixelReadoutTechnology::FEI3) { return 0; }
  if (!m_useByteStreamRD53 && p_design->getReadoutTechnology() == InDetDD::PixelReadoutTechnology::RD53) { return 0; }

  std::scoped_lock<std::mutex> lock{*m_cacheMutex.get(ctx)};
  const auto *idcCachePtr = getCacheEntry(ctx)->IDCCache;
  if (idcCachePtr==nullptr) {
    ATH_MSG_ERROR("PixelConditionsSummaryTool No cache! " );
    return 0;
  }
  uint64_t word = (uint64_t)idcCachePtr->retrieve(index);
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
  uint64_t word = getBSErrorWord(moduleHash,ctx);
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::TruncatedROB))      { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MaskedROB))         { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Preamble))          { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::TimeOut))           { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::LVL1ID))            { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::BCID))              { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Trailer))           { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCLVL1IDEoECheck)) { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCBCIDEoECheck))   { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCLVL1IDCheck))    { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCEoEOverflow))    { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCHitOverflow))    { return true; }
  return false;
}

bool PixelConditionsSummaryTool::hasBSError(const IdentifierHash& moduleHash, Identifier pixid, const EventContext& ctx) const {
  if (hasBSError(moduleHash, ctx)) { return true; }

  int maxHash = m_pixelID->wafer_hash_max();
  Identifier moduleID = m_pixelID->wafer_id(pixid);
  int chFE = m_pixelReadout->getFE(pixid, moduleID);
  if (m_pixelReadout->getModuleType(moduleID)==InDetDD::PixelModuleType::IBL_3D) { chFE=0; }

  int indexFE = (1+chFE)*maxHash+static_cast<int>(moduleHash);    // (FE_channel+1)*2048 + moduleHash
  uint64_t word = getBSErrorWord(moduleHash,indexFE,ctx);
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Preamble))          { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::TimeOut))           { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::LVL1ID))            { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::BCID))              { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Trailer))           { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCLVL1IDEoECheck)) { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCBCIDEoECheck))   { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCLVL1IDCheck))    { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCEoEOverflow))    { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCHitOverflow))    { return true; }
  return false;
}

bool PixelConditionsSummaryTool::hasBSError(const IdentifierHash& moduleHash, const EventContext& ctx, Identifier pixid) const {

  int maxHash = m_pixelID->wafer_hash_max();
  Identifier moduleID = m_pixelID->wafer_id(pixid);
  int chFE = m_pixelReadout->getFE(pixid, moduleID);
  if (m_pixelReadout->getModuleType(moduleID)==InDetDD::PixelModuleType::IBL_3D) { chFE=0; }

  int indexFE = (1+chFE)*maxHash+static_cast<int>(moduleHash);    // (FE_channel+1)*2048 + moduleHash
  uint64_t word = getBSErrorWord(moduleHash,indexFE,ctx);
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Preamble))          { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::TimeOut))           { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::LVL1ID))            { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::BCID))              { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::Trailer))           { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCLVL1IDEoECheck)) { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCBCIDEoECheck))   { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCLVL1IDCheck))    { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCEoEOverflow))    { return true; }
  if (PixelByteStreamErrors::hasError(word,PixelByteStreamErrors::MCCHitOverflow))    { return true; }
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

  // The index array is defined in PixelRawDataProviderTool::SizeOfIDCInDetBSErrContainer()
  // Here, 52736 is a separator beween error flags and isActive flags.
  bool useByteStream = (m_useByteStreamFEI4 || m_useByteStreamFEI3 || m_useByteStreamRD53);
  if (useByteStream && getBSErrorWord(moduleHash,moduleHash+52736,ctx)!=1) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey,ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  if (!m_condTDAQKey.empty() && SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelDeadMapCondData>(m_condDeadMapKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isActive(const IdentifierHash& moduleHash, const Identifier& elementId, const EventContext& ctx) const {

  // The index array is defined in PixelRawDataProviderTool::SizeOfIDCInDetBSErrContainer()
  // Here, 52736 is a separator beween error flags and isActive flags.
  bool useByteStream = (m_useByteStreamFEI4 || m_useByteStreamFEI3 || m_useByteStreamRD53);
  if (useByteStream && getBSErrorWord(moduleHash,moduleHash+52736,ctx)!=1) { return false; }

  SG::ReadCondHandle<PixelDCSStateData> dcsstate_data(m_condDCSStateKey,ctx);
  bool isDCSActive = false;
  for (unsigned int istate=0; istate<m_activeState.size(); istate++) {
    if (m_activeState[istate]==dcsstate_data->getModuleStatus(moduleHash)) { isDCSActive=true; }
  }
  if (!isDCSActive) { return false; }

  if (!m_condTDAQKey.empty() && SG::ReadCondHandle<PixelTDAQData>(m_condTDAQKey,ctx)->getModuleStatus(moduleHash)) { return false; }

  if (SG::ReadCondHandle<PixelDeadMapCondData>(m_condDeadMapKey,ctx)->getModuleStatus(moduleHash)) { return false; }

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

  if (hasBSError(moduleHash, ctx)) { return false; }

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

  if (SG::ReadCondHandle<PixelDeadMapCondData>(m_condDeadMapKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  if (h==InDetConditions::PIXEL_CHIP) {
    if (!checkChipStatus(moduleHash, elementId)) { return false; }
    if (hasBSError(moduleHash, elementId, ctx)) { return false; }
  }

  return true;
}

namespace {
   inline void setStatus(const std::unordered_map<int, int> &status_map, unsigned int status_mask, std::vector<bool> &module_status) {
      for (const std::pair<const int, int> &elm : status_map ) {
         // set modules good if the module status passes the mask.
         module_status.at(elm.first)  = module_status.at(elm.first) || (status_mask & (1<<elm.second));
      }
   }

   inline void andStatus(const std::unordered_map<int, int> &status_map, unsigned int status_mask, std::vector<bool> &module_status) {
      for (const std::pair<const int, int> &elm : status_map ) {
         // set modules good if the module status passes the mask.
         module_status.at(elm.first) = module_status.at(elm.first) && (status_mask & (1<<elm.second));
      }
   }
   inline void andNotStatus(const std::unordered_map<int, int> &status_map, std::vector<bool> &module_status) {
      for (const std::pair<const int, int> &elm : status_map ) {
         // set modules good if the module status passes the mask.
         module_status.at(elm.first) = module_status.at(elm.first) && (elm.second==0);
      }
   }
   bool isBSError(uint64_t error, uint64_t max_error, uint64_t mask) {
      return error<max_error && (error & mask);
   }
}

namespace {
   const InDet::PixelDetectorElementStatus *castToDerived(const InDet::SiDetectorElementStatus *input) {
      const InDet::PixelDetectorElementStatus *ret = dynamic_cast<const InDet::PixelDetectorElementStatus *>(input);
      if (!ret) {
         throw std::runtime_error("Object is not of expected type InDet::PixelDetectorElementStatus");
      }
      return ret;
   }
}
std::unique_ptr<InDet::SiDetectorElementStatus>
PixelConditionsSummaryTool::createDetectorElementStatus(const EventContext& ctx,
                                                        SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const  {
   if (!m_pixelDetElStatusCondKey.empty()) {
      SG::ReadCondHandle<InDet::SiDetectorElementStatus> input_element_status{m_pixelDetElStatusCondKey, ctx};
      if (whandle) {
        whandle->addDependency (input_element_status);
      }
      return std::make_unique<InDet::PixelDetectorElementStatus>(*castToDerived(input_element_status.cptr()));
   }
   else if (!m_pixelDetElStatusEventKey.empty()) {
      SG::ReadHandle<InDet::SiDetectorElementStatus> input_element_status{m_pixelDetElStatusEventKey, ctx};
      return std::make_unique<InDet::PixelDetectorElementStatus>(*castToDerived(input_element_status.cptr()));
   }
   else {
      SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey, ctx);
      if (not pixelDetEleHandle.isValid() ) {
         std::stringstream msg;
         msg << m_pixelDetEleCollKey.fullKey() << " is not available.";
         throw std::runtime_error(msg.str());
      }
      if (whandle) {
        whandle->addDependency (pixelDetEleHandle);
      }
      const InDetDD::SiDetectorElementCollection* elements(*pixelDetEleHandle);
      return std::make_unique<InDet::PixelDetectorElementStatus>(*elements);
   }
}

std::unique_ptr<InDet::SiDetectorElementStatus>
PixelConditionsSummaryTool::getDetectorElementStatus(const EventContext& ctx,
                                                     SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const  {
   std::unique_ptr<InDet::SiDetectorElementStatus> element_status( createDetectorElementStatus(ctx, whandle));
   std::vector<bool> &status=element_status->getElementStatus();
   status.resize(m_pixelID->wafer_hash_max(),
                    ((1<<0) & m_activeStateMask)     // default value of PixelDCSStateData  is 0
                 );
   std::vector<InDet::ChipFlags_t> &chip_status=element_status->getElementChipStatus();
   chip_status.resize(status.size(),0);

   // module state
   {
      SG::ReadCondHandle<PixelDCSStateData> dcs_state_handle(m_condDCSStateKey, ctx);
      setStatus(dcs_state_handle->moduleStatusMap(),   m_activeStateMask,  status);
      if (whandle) {
        whandle->addDependency (dcs_state_handle);
      }
   }
   const bool active_only = m_activeOnly;
   if (!active_only) {
      SG::ReadCondHandle<PixelDCSStatusData> dcs_status_handle(m_condDCSStatusKey, ctx);
      andStatus(dcs_status_handle->moduleStatusMap(), m_activeStatusMask, status);
      if (whandle) {
        whandle->addDependency (dcs_status_handle);
      }
   }
   if (!m_condTDAQKey.empty()) {
      SG::ReadCondHandle<PixelTDAQData> tdaq_handle(m_condTDAQKey,ctx);
      andNotStatus(tdaq_handle->moduleStatusMap(), status);
      if (whandle) {
        whandle->addDependency (tdaq_handle);
      }
   }
   {
      SG::ReadCondHandle<PixelDeadMapCondData> dead_map_handle(m_condDeadMapKey, ctx);
      andNotStatus(dead_map_handle->moduleStatusMap(), status);
      if (whandle) {
        whandle->addDependency (dead_map_handle);
      }

      const PixelDeadMapCondData *dead_map = dead_map_handle.cptr();
      unsigned int element_i=0;
      for (const InDetDD::SiDetectorElement *element : element_status->getDetectorElements()) {
         if (status[element_i]) {
            const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
            InDetDD::PixelReadoutTechnology readout_technology = p_design->getReadoutTechnology();
            unsigned int number_of_chips = readout_technology == InDetDD::PixelReadoutTechnology::FEI3  ? 2*p_design->numberOfCircuits() : p_design->numberOfCircuits();
            InDet::ChipFlags_t  chip_mask = (1ul<<number_of_chips)-1ul;
            assert( chip_mask != 0 );
            std::bitset<16> dead_chip_mask(dead_map->getChipStatus(element_i));
            chip_status[element_i]  =   chip_mask & (~static_cast<InDet::ChipFlags_t>(dead_chip_mask.to_ulong()));
         }
         ++element_i;
      }
   }


   static_assert( static_cast<unsigned int>(InDetDD::PixelReadoutTechnology::FEI4) < 32u) ;
   static_assert( static_cast<unsigned int>(InDetDD::PixelReadoutTechnology::FEI3) < 32u) ;
   static_assert( static_cast<unsigned int>(InDetDD::PixelReadoutTechnology::RD53) < 32u) ;
   unsigned int readout_technology_mask=  Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::FEI4, 1 && m_useByteStreamFEI4 )
      | Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::FEI3, 1 && m_useByteStreamFEI3 )
      | Pixel::makeReadoutTechnologyBit(InDetDD::PixelReadoutTechnology::RD53, 1 && m_useByteStreamRD53 );
   if (readout_technology_mask) {
      if (whandle) {
        ATH_MSG_ERROR("PixelConditionsSummaryTool not configured for use with conditions objects");
        whandle->addDependency (IDetectorElementStatusTool::getInvalidRange());
      }
      std::scoped_lock<std::mutex> lock{*m_cacheMutex.get(ctx)};
      const IDCInDetBSErrContainer_Cache *idcCachePtr = nullptr;
      idcCachePtr = getCacheEntry(ctx)->IDCCache;
      if (idcCachePtr==nullptr) {
         ATH_MSG_ERROR("PixelConditionsSummaryTool No cache! " );
      }
      else {

         constexpr uint64_t error_mask = PixelByteStreamErrors::makeError(PixelByteStreamErrors::TruncatedROB)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MaskedROB)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::Preamble)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::TimeOut)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::LVL1ID)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::BCID)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::Trailer)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCLVL1IDEoECheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCBCIDEoECheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCLVL1IDCheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCEoEOverflow)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCHitOverflow);

         constexpr uint64_t chip_error_mask = PixelByteStreamErrors::makeError(PixelByteStreamErrors::Preamble)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::TimeOut)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::LVL1ID)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::BCID)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::Trailer)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCLVL1IDEoECheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCBCIDEoECheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCLVL1IDCheck)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCEoEOverflow)
            | PixelByteStreamErrors::makeError(PixelByteStreamErrors::MCCHitOverflow);

         // The index array is defined in PixelRawDataProviderTool::SizeOfIDCInDetBSErrContainer()
         // Here, 52736 is a separator beween error flags and isActive flags.
         unsigned int element_offset_i = (active_only ? 52736 : 0);

         unsigned int element_i=0;
         unsigned int maxHash = m_pixelID->wafer_hash_max();
         for (const InDetDD::SiDetectorElement *element : element_status->getDetectorElements()) {
            const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
            InDetDD::PixelReadoutTechnology readout_technology = p_design->getReadoutTechnology();
            unsigned int readout_technology_flags = readout_technology_mask & Pixel::makeReadoutTechnologyBit(readout_technology);

            // set to false if has one of the considered errors and the readout technology is considered.
            status.at(element_i) = status.at(element_i) &&  not (   readout_technology_flags
                                                                &&  ( !active_only
                                                                     ?  isBSError(static_cast<uint64_t>(idcCachePtr->retrieve(element_i)), m_missingErrorInfo, error_mask)
                                                                      : idcCachePtr->retrieve(element_i+element_offset_i)!=1 ));

            // compute the status for the individual front-ends
            if (status[element_i]) {
               unsigned int number_of_chips = readout_technology == InDetDD::PixelReadoutTechnology::FEI3  ? 2*p_design->numberOfCircuits() : p_design->numberOfCircuits();
               InDet::ChipFlags_t  chip_mask = status[element_i] ? (1ul<<number_of_chips)-1ul : 0;
               assert( chip_mask != 0 );
               InDet::ChipFlags_t bs_status_flags = active_only ? chip_mask : 0;
               if (!active_only) {
                  for (unsigned int chip_i =0; chip_i < number_of_chips; ++chip_i) {
                     // get bytestream error for chip
                     unsigned int indexFE = (1+chip_i)*maxHash + element_i;    // (FE_channel+1)*2048 + moduleHash
                     InDet::ChipFlags_t chip_flag =not (    readout_technology_flags
                                                        && (isBSError(static_cast<uint64_t>(idcCachePtr->retrieve(indexFE)), m_missingErrorInfo, chip_error_mask)));
                     bs_status_flags |= (chip_flag << chip_i);
                  }
               }
               chip_status[element_i] &=  (bs_status_flags & chip_mask);
            }
            else {
               chip_status[element_i] = 0;
            }
            ++element_i;
         }
      }
   }
   return element_status;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash& moduleHash, const EventContext& ctx) const {

  if (hasBSError(moduleHash, ctx)) { return false; }

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

  if (SG::ReadCondHandle<PixelDeadMapCondData>(m_condDeadMapKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  return true;
}

bool PixelConditionsSummaryTool::isGood(const IdentifierHash & moduleHash, const Identifier &elementId, const EventContext& ctx) const {

  if (hasBSError(moduleHash, ctx)) { return false; }

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

  if (SG::ReadCondHandle<PixelDeadMapCondData>(m_condDeadMapKey, ctx)->getModuleStatus(moduleHash)) { return false; }

  if (!checkChipStatus(moduleHash, elementId)) { return false; }

  if (hasBSError(moduleHash, ctx, elementId)) { return false; }

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
        if (!hasBSError(moduleHash, m_pixelID->pixel_id(moduleID,i,j), ctx)) { nGood++; }
      }
    }
  }
  return nGood/nTotal;
}

