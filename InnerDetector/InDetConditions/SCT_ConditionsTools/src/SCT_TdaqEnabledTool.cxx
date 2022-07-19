/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_TdaqEnabledTool.cxx
 * implementation file for service allowing one to declare rods as bad, or read the bad rods from the Tdaq entries in COOL db
 * @author shaun.roe@cern.ch
**/

#include "SCT_TdaqEnabledTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_DetectorElementStatus.h"

#include "InDetIdentifier/SCT_ID.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"

// Constructor
SCT_TdaqEnabledTool::SCT_TdaqEnabledTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent)
{
}

//Initialize
StatusCode 
SCT_TdaqEnabledTool::initialize() {
  const std::string databaseUseString{m_useDatabase?"":"not "};
  ATH_MSG_INFO(" Database will "<<databaseUseString<<"be used.");

  ATH_CHECK(detStore()->retrieve(m_pHelper,"SCT_ID"));
  // Read Cond Handle Key
  ATH_CHECK(m_condKey.initialize(m_useDatabase));

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_TdaqEnabledTool::finalize() {
  return StatusCode::SUCCESS;
}

bool 
SCT_TdaqEnabledTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::DEFAULT or h==InDetConditions::SCT_SIDE or h==InDetConditions::SCT_MODULE); 
}

bool 
SCT_TdaqEnabledTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return true;
  //turn to hash, given the identifier
  const IdentifierHash hashId{m_pHelper->wafer_hash(elementId)};
  return isGood(hashId, ctx);
}

bool 
SCT_TdaqEnabledTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

bool
SCT_TdaqEnabledTool::isGood(const IdentifierHash& hashId, const EventContext& ctx) const {
  const SCT_TdaqEnabledCondData* condData{getCondData(ctx)};
  if (condData==nullptr) return false;
  return condData->isGood(hashId);
}

bool
SCT_TdaqEnabledTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(hashId, ctx);
}

void
SCT_TdaqEnabledTool::getDetectorElementStatus(const EventContext& ctx, InDet::SiDetectorElementStatus &element_status, 
                                              SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const {
  SG::ReadCondHandle<SCT_TdaqEnabledCondData> condDataHandle{m_condKey, ctx};
  if (not condDataHandle.isValid()) {
     ATH_MSG_ERROR("Invalid cond data handle " << m_condKey.key() );
     return;
  }
  if (whandle) {
    whandle->addDependency (condDataHandle);
  }
  const SCT_TdaqEnabledCondData* condData{condDataHandle.cptr()};
  if (condData==nullptr) {
    ATH_MSG_ERROR("Invalid TdaqEnabledCondData. Return true.");
    return ;
  }
  std::vector<bool> &status = element_status.getElementStatus();
  if (status.empty()) {
     status.resize(m_pHelper->wafer_hash_max(),true);
  }
  if (not condData->isNoneBad()) {
     std::vector<bool> tdaq_enabled;
     tdaq_enabled.resize(m_pHelper->wafer_hash_max(),false);
     for (const IdentifierHash &id_hash :  condData->goodIdHashes() ) {
        tdaq_enabled.at(id_hash.value())=true;
     }
     for (unsigned int status_i=0; status_i<status.size(); ++status_i) {
        status[status_i] = status[status_i] && tdaq_enabled.at(status_i);
     }
  }

}

const SCT_TdaqEnabledCondData*
SCT_TdaqEnabledTool::getCondData(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_TdaqEnabledCondData> condData{m_condKey, ctx};
  return condData.retrieve();
}
