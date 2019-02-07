/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoTool.cxx
 * implementation file for tool allowing one to declare modules as bad
 * @author daiki.hayakawa@cern.ch
 **/

#include "SCT_RODVetoTool.h"
//STL includes
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <sstream>

//Athena includes

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"

//StoreGate Includes
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "CxxUtils/make_unique.h"

// Constructor
SCT_RODVetoTool::SCT_RODVetoTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_badModuleIds{"BadSCTModuleIds_RODVeto"},
  m_pHelper{nullptr}
  {
    declareProperty("BadModuleIds", m_badModuleIds, "Read key for bad module identifiers");
  }

//Initialize
StatusCode 
SCT_RODVetoTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_pHelper, "SCT_ID"));
  ATH_CHECK(m_badModuleIds.initialize());
 
  return  StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_RODVetoTool::finalize() {
  return StatusCode::SUCCESS;
}

bool 
SCT_RODVetoTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return ((h==InDetConditions::DEFAULT) or (h==InDetConditions::SCT_SIDE));
}

bool 
SCT_RODVetoTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return true;
  const IdentifierSet* badIds{getCondData(ctx)};
  if (badIds==nullptr) {
    ATH_MSG_ERROR("IdentifierSet cannot be retrieved in isGood. true is returned.");
    return true;
  }
  bool result{badIds->find(elementId) == badIds->end()};
  return result;
}

bool 
SCT_RODVetoTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

bool 
SCT_RODVetoTool::isGood(const IdentifierHash& hashId, const EventContext& ctx) const {
  Identifier elementId{m_pHelper->wafer_id(hashId)};
  Identifier moduleId{m_pHelper->module_id(elementId)};
  return isGood(moduleId, ctx);
}

bool 
SCT_RODVetoTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(hashId, ctx);
}

const IdentifierSet*
SCT_RODVetoTool::getCondData(const EventContext& ctx) const {
  SG::ReadHandle<IdentifierSet> condData{m_badModuleIds, ctx};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_badModuleIds.key());
    return nullptr;
  }
  return condData.cptr();
}
