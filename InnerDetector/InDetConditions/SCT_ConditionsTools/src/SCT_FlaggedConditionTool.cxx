/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_FlaggedConditionTool.h"

// Athena
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_FlaggedConditionTool::SCT_FlaggedConditionTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent)
{
}

// Initialize
StatusCode SCT_FlaggedConditionTool::initialize() {
  ATH_MSG_INFO("SCT_FlaggedConditionTool::initialize()");

  // Retrieve SCT helper
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT helper failed to retrieve");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_badIds.initialize());

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_FlaggedConditionTool::finalize() {
  ATH_MSG_INFO("SCT_FlaggedConditionTool::finalize()");
  return StatusCode::SUCCESS;
}

// Detector elements that this service can report about
bool SCT_FlaggedConditionTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h == InDetConditions::SCT_SIDE or h == InDetConditions::DEFAULT);
}

// Is this element good (by Identifier)?
bool SCT_FlaggedConditionTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return true;
  const IdentifierHash hashId{m_sctID->wafer_hash(elementId)};
  return isGood(hashId, ctx);
}

bool SCT_FlaggedConditionTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

// Is this element good (by IdentifierHash)?
bool SCT_FlaggedConditionTool::isGood(const IdentifierHash& hashId, const EventContext& ctx) const {
  const IDCInDetBSErrContainer* badIds{getCondData(ctx)};
  if (badIds==nullptr) {
    if (m_numWarnForFailures<m_maxNumWarnForFailures) {
      ATH_MSG_WARNING(m_badIds.key() << " cannot be retrieved. (isGood)");
      m_numWarnForFailures++;
      if (m_numWarnForFailures==m_maxNumWarnForFailures) {
        ATH_MSG_WARNING("Disabling this type of messages from " << name());
      }
    }
    return false;
  }

  return (not badIds->present(hashId));
}

bool SCT_FlaggedConditionTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(hashId, ctx);
}

// Retrieve the reason why the wafer is flagged as bad (by IdentifierHash)
// If wafer is not found return a null string
const std::string& SCT_FlaggedConditionTool::details(const IdentifierHash& hashId, const EventContext& ctx) const {
  static const std::string nullString;

  const IDCInDetBSErrContainer* badIds{getCondData(ctx)};
  if (badIds==nullptr) {
    if (m_numWarnForFailures<m_maxNumWarnForFailures) {
      ATH_MSG_WARNING(m_badIds.key() << " cannot be retrieved. (details)");
      m_numWarnForFailures++;
      if (m_numWarnForFailures==m_maxNumWarnForFailures) {
        ATH_MSG_WARNING("Disabling this type of messages from " << name());
      }
    }
    return nullString;
  }

  if (badIds->present(hashId)) {
    const IDCInDetBSErrContainer::ErrorCode value{badIds->retrieve(hashId)};
    if (value>0) {
      for (size_t error{0}; error<SCT_FlaggedCondEnum::NUM_ERROR_TYPES; error++) {
        if (value & (1 << error)) return SCT_FlaggedCondEnum::ErrorTypeDescription[error];
      }
    }
  }
  return nullString;
}

const std::string& SCT_FlaggedConditionTool::details(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return details(hashId, ctx);
}

// Retrieve the reason why the wafer is flagged as bad (by Identifier)
// If wafer is not found return a null string
const std::string& SCT_FlaggedConditionTool::details(const Identifier& Id, const EventContext& ctx) const {
  const IdentifierHash hashId{m_sctID->wafer_hash(Id)};
  return details(hashId, ctx);
}

const std::string& SCT_FlaggedConditionTool::details(const Identifier& Id) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return details(Id, ctx);
}

int SCT_FlaggedConditionTool::numBadIds(const EventContext& ctx) const {
  const IDCInDetBSErrContainer* badIds{getCondData(ctx)};
  if (badIds==nullptr) {
    if (m_numWarnForFailures<m_maxNumWarnForFailures) {
      ATH_MSG_WARNING(m_badIds.key() << " cannot be retrieved. (numBadIds)");
      m_numWarnForFailures++;
      if (m_numWarnForFailures==m_maxNumWarnForFailures) {
        ATH_MSG_WARNING("Disabling this type of messages from " << name());
      }
    }
    return -1;
  }

  return static_cast<int>(badIds->numberSet());
}

int SCT_FlaggedConditionTool::numBadIds() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return numBadIds(ctx);
}

const IDCInDetBSErrContainer* SCT_FlaggedConditionTool::getBadIds(const EventContext& ctx) const {
  return getCondData(ctx);
}

const IDCInDetBSErrContainer* SCT_FlaggedConditionTool::getBadIds() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getBadIds(ctx);
}

const IDCInDetBSErrContainer* SCT_FlaggedConditionTool::getCondData(const EventContext& ctx) const {
  SG::ReadHandle<IDCInDetBSErrContainer> condData{m_badIds, ctx};
  if (not condData.isValid()) {
    if (m_numWarnForFailures<m_maxNumWarnForFailures) {
      ATH_MSG_WARNING("Failed to get " << m_badIds.key());
      m_numWarnForFailures++;
      if (m_numWarnForFailures==m_maxNumWarnForFailures) {
        ATH_MSG_WARNING("Disabling this type of messages from " << name());
      }
    }
    return nullptr;
  }
  return condData.cptr();
}
