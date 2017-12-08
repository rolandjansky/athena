/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ConfigurationConditionsSvc.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"

// Athena includes
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_Cabling/SCT_SerialNumber.h"

// Local includes
#include "SCT_Chip.h"

// in case the chip number cannot be retrieved, this is the invalid value
static const int invalidChipNumber(-1);
// Constructor
SCT_ConfigurationConditionsSvc::SCT_ConfigurationConditionsSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_mutex{},
  m_cache{},
  m_condData{},
  m_condKey{std::string{"SCT_ConfigurationCondData"}},
  m_detStore{"DetectorStore", name},
  m_pHelper{nullptr},
  m_cablingSvc{"SCT_CablingSvc", name},
  m_pManager{nullptr},
  m_checkStripsInsideModules{true}
{ 
  declareProperty("checkStripsInsideModule", m_checkStripsInsideModules);
}

// Initialize
StatusCode SCT_ConfigurationConditionsSvc::initialize() {
  ATH_MSG_INFO("Initializing configuration");

  ATH_CHECK(m_cablingSvc.retrieve());
  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_detStore->retrieve(m_pManager, "SCT"));
  ATH_CHECK(m_detStore->retrieve(m_pHelper, "SCT_ID"));

  // Read Cond Handle Key
  ATH_CHECK(m_condKey.initialize());

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_ConfigurationConditionsSvc::finalize() {
  ATH_MSG_INFO("Configuration finalize");

  return StatusCode::SUCCESS;
}

// Query interfaces.
StatusCode SCT_ConfigurationConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_ConfigurationConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = this;
  } else if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

// What level of element can this service report about
bool SCT_ConfigurationConditionsSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return (h == InDetConditions::SCT_STRIP or
          h == InDetConditions::SCT_CHIP or
          h == InDetConditions::SCT_SIDE or
          h == InDetConditions::SCT_MODULE or
          h == InDetConditions::DEFAULT);
}

// Is an element with this Identifier and hierachy good?
bool SCT_ConfigurationConditionsSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  if (not canReportAbout(h)) return true;

  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In isGood, SCT_ConfigurationCondData pointer cannot be retrieved");
    return false;
  }

  bool result{true};
  if (h == InDetConditions::SCT_STRIP) {
    result = (not condData->isBadStripId(elementId));
    // If strip itself is not bad, check if it's in a bad module
    if (result and m_checkStripsInsideModules) result = (not isStripInBadModule(elementId));
  } else if (h == InDetConditions::SCT_MODULE) {
    result = (not condData->isBadModuleId(elementId));
  } else if (h == InDetConditions::SCT_SIDE or h == InDetConditions::DEFAULT) {
    result = (not condData->isBadWaferId(elementId));
  } else if (h == InDetConditions::SCT_CHIP) {
    result = isGoodChip(elementId);
  }
  return result;
}

// Is a wafer with this IdentifierHash good?
bool SCT_ConfigurationConditionsSvc::isGood(const IdentifierHash& hashId) {
  Identifier elementId{m_pHelper->wafer_id(hashId)};
  return isGood(elementId);
}

// Is a chip with this Identifier good?
bool SCT_ConfigurationConditionsSvc::isGoodChip(const Identifier& stripId) const {
  // This check assumes present SCT.
  // Get module number
  const Identifier moduleId{m_pHelper->module_id(stripId)};
  if (not moduleId.is_valid()) {
    ATH_MSG_WARNING("moduleId obtained from stripId " << stripId << " is invalid.");
    return false;
  }

  // badChips word for the module
  const unsigned int v_badChips{badChips(moduleId)};
  // badChips holds 12 bits.
  // bit 0 (LSB) is chip 0 for side 0.
  // bit 5 is chip 5 for side 0.
  // bit 6 is chip 6 for side 1.
  // bit 11 is chip 11 for side 1.

  // If there is no bad chip, this check is done.
  if (v_badChips==0) return true;

  const int side{m_pHelper->side(stripId)};
  // Check the six chips on the side
  // 0x3F  = 0000 0011 1111
  // 0xFC0 = 1111 1100 0000
  // If there is no bad chip on the side, this check is done.
  if ((side==0 and (v_badChips & 0x3F)==0) or (side==1 and (v_badChips & 0xFC0)==0)) return true;

  int chip{getChip(stripId)};
  if (chip<0 or chip>=12) {
    ATH_MSG_WARNING("chip number is invalid: " << chip);
    return false;
  }

  // Check if the chip is bad
  const bool badChip{static_cast<bool>(v_badChips & (1<<chip))};

  return (not badChip);
}

  // Callback funtion (with arguments required by IovDbSvc) to fill channel, module and link info
StatusCode SCT_ConfigurationConditionsSvc::fillData(int& /*i*/, std::list<std::string>& /*l*/) {  
  return StatusCode::SUCCESS;
}

// Is the information filled?
bool SCT_ConfigurationConditionsSvc::filled() const{
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) return false;
  return true;
}

// Check if a strip is within a bad module
bool SCT_ConfigurationConditionsSvc::isStripInBadModule(const Identifier& stripId) {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In isStripInBadModule, SCT_ConfigurationCondData pointer cannot be retrieved");
    return true;
  }

  const Identifier moduleId(m_pHelper->module_id(m_pHelper->wafer_id(stripId)));
  return condData->isBadModuleId(moduleId);
}

// Check if a wafer is within a bad module
bool SCT_ConfigurationConditionsSvc::isWaferInBadModule(const Identifier& waferId) {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In isWaferInBadModule, SCT_ConfigurationCondData pointer cannot be retrieved");
    return true;
  }

  Identifier moduleId{m_pHelper->module_id(waferId)};
  return condData->isBadModuleId(moduleId);
}

// Find the chip number containing a particular strip Identifier
int SCT_ConfigurationConditionsSvc::getChip(const Identifier& stripId) const {

  // Find side and strip number
  const int side{m_pHelper->side(stripId)};
  int strip{m_pHelper->strip(stripId)};

  // Check for swapped readout direction
  const IdentifierHash waferHash{m_pHelper->wafer_hash(m_pHelper->wafer_id(stripId))};
  const InDetDD::SiDetectorElement* pElement{m_pManager->getDetectorElement(waferHash)};
  if (pElement==nullptr) {
    ATH_MSG_FATAL("Element pointer is NULL in 'badStrips' method");
    return invalidChipNumber;
  }
  strip = (pElement->swapPhiReadoutDirection()) ? lastStrip - strip: strip;

  // Find chip number
  return (side==0 ? strip/stripsPerChip : strip/stripsPerChip + 6);
}

const std::set<Identifier>* SCT_ConfigurationConditionsSvc::badModules() {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badModules, SCT_ConfigurationCondData pointer cannot be retrieved");
    return nullptr;
  }

  return condData->getBadModuleIds();
}

void SCT_ConfigurationConditionsSvc::badStrips(const Identifier& moduleId,  std::set<Identifier>& strips, bool ignoreBadModules, bool ignoreBadChips) {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badStrips, SCT_ConfigurationCondData pointer cannot be retrieved");
    return;
  }

  // Bad strips for a given module
  if (ignoreBadModules) {
    // Ignore strips in bad modules
    if (condData->isBadModuleId(moduleId)) return;
  }

  std::set<Identifier>::const_iterator chanItr{condData->getBadStripIds()->begin()};
  std::set<Identifier>::const_iterator chanEnd{condData->getBadStripIds()->end()};
  for (; chanItr != chanEnd; ++chanItr) {
    if (ignoreBadChips) {
      // Ignore strips in bad chips
      int chip{getChip(*chanItr)};
      if (chip!=invalidChipNumber) {
        unsigned int chipStatusWord{condData->getBadChips(moduleId)};
        if ((chipStatusWord & (1 << chip)) != 0) continue;
      }   
    }
    if (m_pHelper->module_id(m_pHelper->wafer_id((*chanItr))) == moduleId) strips.insert(*chanItr);
  }
}
       
std::pair<bool, bool> SCT_ConfigurationConditionsSvc::badLinks(const Identifier& moduleId) {
  // Bad links for a given module
  // Bad convetion is used. true is for good link and false is for bad link...
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badLinks, SCT_ConfigurationCondData pointer cannot be retrieved");
    return std::pair<bool, bool>{false, false};
  }

  return condData->areBadLinks(moduleId);
}

const std::map<Identifier, std::pair<bool, bool>>* SCT_ConfigurationConditionsSvc::badLinks() {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badLinks, SCT_ConfigurationCondData pointer cannot be retrieved");
    return nullptr;
  }

  return condData->getBadLinks();
}

const std::map<Identifier, unsigned int>* SCT_ConfigurationConditionsSvc::badChips() {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badChips, SCT_ConfigurationCondData pointer cannot be retrieved");
    return nullptr;
  }

  return condData->getBadChips();
}

unsigned int SCT_ConfigurationConditionsSvc::badChips(const Identifier& moduleId) const {
  // Bad chips for a given module
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badChips, SCT_ConfigurationCondData pointer cannot be retrieved");
    return 0xFFF; // 12 bad chips
  }

  return condData->getBadChips(moduleId);
}

void 
SCT_ConfigurationConditionsSvc::badStrips(std::set<Identifier>& strips, bool ignoreBadModules, bool ignoreBadChips) {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badStrips, SCT_ConfigurationCondData pointer cannot be retrieved");
    return;
  }

  if (ignoreBadModules == false and ignoreBadChips == false) {
    std::copy(condData->getBadStripIds()->begin(), condData->getBadStripIds()->end(), std::inserter(strips,strips.begin()));
    return;
  }
  std::set<Identifier>::const_iterator chanItr{condData->getBadStripIds()->begin()};
  std::set<Identifier>::const_iterator chanEnd{condData->getBadStripIds()->end()};
  for (; chanItr != chanEnd; ++chanItr) {
    Identifier moduleId{m_pHelper->module_id(m_pHelper->wafer_id(*chanItr))};
    // Ignore strips in bad modules
    if (ignoreBadModules) {
      if (condData->isBadModuleId(moduleId)) continue;
    }
    // Ignore strips in bad chips
    if (ignoreBadChips) {
      int chip{getChip(*chanItr)};
      if (chip !=invalidChipNumber) {
        unsigned int chipStatusWord{condData->getBadChips(moduleId)};
        if ((chipStatusWord & (1 << chip)) != 0) continue;
      }
    }
    strips.insert(*chanItr);
  }
}

const SCT_ConfigurationCondData*
SCT_ConfigurationConditionsSvc::getCondData(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cache.size()) {
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    SG::ReadCondHandle<SCT_ConfigurationCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
