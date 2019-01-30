/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_StripVetoTool.cxx
 * implementation file for tool allowing one to declare strips as bad
 * @author shaun.roe@cern.ch
**/

#include "SCT_StripVetoTool.h"

// STL includes
#include <vector>

// Athena include
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_StripVetoTool::SCT_StripVetoTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent),
  m_pHelper{nullptr} {
  declareProperty("BadStripIdentifiers", m_badElements); // Identifiers should be given as strings and decimal.
}

//Initialize
StatusCode 
SCT_StripVetoTool::initialize() {
  if (fillData().isFailure()) {
    ATH_MSG_ERROR("Failed to fill data");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Initialized SCT_StripVetoTool with " << m_badElements.value().size() << " elements declared bad.");

  if (detStore()->retrieve(m_pHelper, "SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT helper failed to retrieve");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_StripVetoTool::finalize() {
  return StatusCode::SUCCESS;
}

bool 
SCT_StripVetoTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::SCT_STRIP); //default case is the side, which we cant report on
}

bool 
SCT_StripVetoTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return true;
  return (m_badIds.find(elementId) == m_badIds.end());
}

bool 
SCT_StripVetoTool::isGood(const Identifier& elementId, const EventContext& /*ctx*/, InDetConditions::Hierarchy h) const {
  return isGood(elementId, h);
}

bool 
SCT_StripVetoTool::isGood(const IdentifierHash& /*hashId*/) const { //comment out unused parameter to prevent compiler warning
  return true; //cant answer questions about the module side
}

bool 
SCT_StripVetoTool::isGood(const IdentifierHash& hashId, const EventContext& /*ctx*/) const {
  return isGood(hashId);
}

StatusCode 
SCT_StripVetoTool::fillData() {
  if (m_badElements.value().empty()) ATH_MSG_INFO("No bad strips.");
  std::vector<std::string>::const_iterator pId{m_badElements.value().begin()};
  std::vector<std::string>::const_iterator last{m_badElements.value().end()};
  bool success{true};
  for(; pId not_eq last; ++pId) {
    success &= m_badIds.insert(Identifier(static_cast<Identifier::value_type>(atoll(pId->c_str())))).second;
  }
  return (success ? StatusCode::SUCCESS : StatusCode::FAILURE);
}
