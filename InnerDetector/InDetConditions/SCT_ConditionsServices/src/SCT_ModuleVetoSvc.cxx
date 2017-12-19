/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ModuleVetoSvc.cxx
 * implementation file for service allowing one to declare modules as bad
 * @author shaun.roe@cern.ch
 **/

#include "SCT_ModuleVetoSvc.h"

//STL includes
#include <algorithm>
#include <iterator>

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"

static const std::string databaseSignature{"database"};

// Constructor
SCT_ModuleVetoSvc::SCT_ModuleVetoSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_localCondData{},
  m_pHelper{nullptr},
  m_useDatabase{false},
  m_maskLayers{false},
  m_maskSide{-1},
  m_detStore{"DetectorStore", name},
  m_mutex{},
  m_cache{},
  m_condData{},
  m_condKey{"SCT_ModuleVetoCondData"}
  {
    declareProperty("BadModuleIdentifiers", m_badElements);
    declareProperty("MaskLayers",  m_maskLayers, "Mask full layers/disks in overlay" ); 
    declareProperty("MaskSide",  m_maskSide, "Mask full modules (-1), innwe (0) or outer (1) sides" );  
    declareProperty("LayersToMask", m_layersToMask, "Which barrel layers to mask out, goes from 0 to N-1"); 
    declareProperty("DisksToMask", m_disksToMask, "Which endcap disks to mask out, goes from -N+1 to N+1 , skipping zero");
  }

//Initialize
StatusCode 
SCT_ModuleVetoSvc::initialize() {
  if (m_maskLayers and !m_layersToMask.size() and !m_disksToMask.size()) {
    ATH_MSG_INFO("Layer/Disk masking enabled, but no layer/disk specified!");
    m_maskLayers = false;
  }
  
  if (!m_maskLayers and (m_layersToMask.size() or m_disksToMask.size())) {
    ATH_MSG_INFO("Layer/Disk to mask specified, but masking is disabled!");
  } 

  if (!m_maskLayers and m_maskSide!=-1) {
    ATH_MSG_INFO("Layer/Disk side to mask specified, but masking is disabled!");
  } 
  
  if (m_maskLayers and m_disksToMask.size() and (std::find(m_disksToMask.begin(), m_disksToMask.end(),0)!=m_disksToMask.end())) {
    ATH_MSG_WARNING("0th Disk not defined (-N to N) - check your setup!");
  }   

  if (m_detStore->retrieve(m_pHelper, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL("SCT helper failed to retrieve");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Successfully retrieved SCT_ID helper");
  }

  // If "database" is found in m_badElements, COOL database is used.
  m_useDatabase=(std::find(m_badElements.value().begin(), m_badElements.value().end(), databaseSignature) != m_badElements.value().end());
  ATH_MSG_INFO("m_useDatabase is " << m_useDatabase);

  if (not m_useDatabase) {
    if (fillData().isFailure()) {
      ATH_MSG_FATAL("Failed to fill data");
      return StatusCode::FAILURE;
    }
  } else {
    // Read Cond Handle
    CHECK(m_condKey.initialize());
  }

  const std::string databaseUseString{m_useDatabase ? "" : "not "};
  ATH_MSG_INFO("Initialized veto service with data, "
               << (m_badElements.value().size() - static_cast<long unsigned int>(m_useDatabase))
               << " elements declared bad. Database will " << databaseUseString << "be used.");

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_ModuleVetoSvc::finalize() {
  return StatusCode::SUCCESS;
}

// From s.binet
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
SCT_ModuleVetoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool 
SCT_ModuleVetoSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return ((h==InDetConditions::DEFAULT) or (h==InDetConditions::SCT_SIDE));
}

bool 
SCT_ModuleVetoSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  if (not canReportAbout(h)) return true;

  // Bad wafer in properties
  if (m_localCondData.isBadWaferId(elementId)) return false;
  // If database is not used, all wafer IDs here should be good.
  if (not m_useDatabase) return true;

  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ModuleVetoCondData* condData{getCondData(ctx)};
  // If database cannot be retrieved, all wafer IDs are good.
  if (condData==nullptr) return true;

  // Return the result of database
  return (not condData->isBadWaferId(elementId));
}

bool 
SCT_ModuleVetoSvc::isGood(const IdentifierHash& hashId) {
  Identifier elementId{m_pHelper->wafer_id(hashId)};
  return isGood(elementId);
}

StatusCode 
SCT_ModuleVetoSvc::fillData() {
  // Reset SCT_ModuleVetoCondData
  m_localCondData.clear();

  // Fill data based on properties
  StatusCode sc{StatusCode::SUCCESS};
  if ((m_badElements.value().size() - static_cast<int>(m_useDatabase)) == 0 and !m_maskLayers) {
    ATH_MSG_INFO("No bad modules in job options.");
    return sc;
  } 

  bool success{true};
  std::vector<std::string>::const_iterator pId{m_badElements.value().begin()};
  std::vector<std::string>::const_iterator last{m_badElements.value().end()};
  for(; pId not_eq last;++pId) {
    unsigned long long idToWrite{static_cast<unsigned long long>(atoll(pId->c_str()))};
    if (*pId != databaseSignature) success &= m_localCondData.setBadWaferId(Identifier(idToWrite));
  }

  if (m_maskLayers) {
    ATH_MSG_INFO("Masking " << m_layersToMask.size() << " SCT Layers");
    ATH_MSG_INFO("Masking " << m_disksToMask.size() << " SCT Disks");
    for(unsigned int i{0}; i < m_pHelper->wafer_hash_max(); i++) {
      Identifier mID{m_pHelper->wafer_id(i)};
      int bec{m_pHelper->barrel_ec(mID)};
      int side{m_pHelper->side(mID)};
      int layer_disk{m_pHelper->layer_disk(mID)};

      if ((bec ==  0 and (m_maskSide==-1 or side==m_maskSide) and (std::find(m_layersToMask.begin(), m_layersToMask.end(),    layer_disk     ) != m_layersToMask.end())) or
          (bec ==  2 and (m_maskSide==-1 or side==m_maskSide) and (std::find(m_disksToMask.begin(),  m_disksToMask.end(),    (layer_disk + 1)) != m_disksToMask.end()))  or
          (bec == -2 and (m_maskSide==-1 or side==m_maskSide) and (std::find(m_disksToMask.begin(),  m_disksToMask.end(), -1*(layer_disk + 1)) != m_disksToMask.end()))) {
        ATH_MSG_DEBUG("Masking ID Hash " << i);
        m_localCondData.setBadWaferId(mID);
      }
    }
  }
  
  ATH_MSG_INFO(m_localCondData.size() << " bad wafers are defined via properties.");

  m_localCondData.setFilled();
  ATH_MSG_DEBUG("Successfully filled bad SCT identifiers list");
  return (success ? sc : StatusCode::FAILURE);
}

StatusCode 
SCT_ModuleVetoSvc::fillData(int& /*i*/, std::list<std::string>& /*folderList*/) {
  return StatusCode::SUCCESS;
}

bool 
SCT_ModuleVetoSvc::canFillDuringInitialize() {
  return (not m_useDatabase); // can only fill during intialize if we don't use the database
}

bool
SCT_ModuleVetoSvc::filled() const {
  // If bad wafer IDs from properties are not filled, not filled.
  if (not m_localCondData.filled()) return false;
  // If database is not used, filled is returned.
  if (not m_useDatabase) return true;

  // If database is used, null pointer means not filled.
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  const SCT_ModuleVetoCondData* condData{getCondData(ctx)};
  return (condData!=nullptr);
}

const SCT_ModuleVetoCondData*
SCT_ModuleVetoSvc::getCondData(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  std::lock_guard<std::mutex> lock{m_mutex};
  if (slot>=m_cache.size()) {
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    SG::ReadCondHandle<SCT_ModuleVetoCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}
