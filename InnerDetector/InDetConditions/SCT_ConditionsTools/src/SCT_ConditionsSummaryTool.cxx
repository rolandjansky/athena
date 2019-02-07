/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummaryTool.cxx
 * implementation file for SCT_ConditionsSummaryTool class
 * @author shaun.roe@cern.ch
 **/

#include "SCT_ConditionsSummaryTool.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

#include "GaudiKernel/EventContext.h"

using namespace std;

// Constructor
SCT_ConditionsSummaryTool::SCT_ConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_toolHandles{this},
  m_noReports{true} {
  declareProperty("ConditionsTools", m_toolHandles);
}

//Initialize
StatusCode 
SCT_ConditionsSummaryTool::initialize() {
  StatusCode sc{m_toolHandles.retrieve()};
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Retrieval of ToolHandleArray<ISCT_ConditionsTool> failed.");
    return sc;
  }

  m_noReports = m_toolHandles.empty();
  return sc;
}

//Finalize
StatusCode
SCT_ConditionsSummaryTool::finalize() {
  ATH_MSG_INFO("Thank-you for using the SCT_ConditionsSummaryTool, version " << PACKAGE_VERSION);
  return StatusCode::SUCCESS;
}

bool
SCT_ConditionsSummaryTool::isActive(const Identifier& elementId, const InDetConditions::Hierarchy h) const {
  return isGood(elementId, h);
}

bool
SCT_ConditionsSummaryTool::isActive(const IdentifierHash& elementHash) const {
  return isGood(elementHash);
}

bool
SCT_ConditionsSummaryTool::isActive(const IdentifierHash& elementHash, const Identifier& elementId) const {
  return isGood(elementHash, elementId);
}

double
SCT_ConditionsSummaryTool::activeFraction(const IdentifierHash& elementHash, const Identifier& idStart, const Identifier& idEnd) const {
  return goodFraction(elementHash, idStart, idEnd);
}

bool
SCT_ConditionsSummaryTool::isGood(const Identifier& elementId, const InDetConditions::Hierarchy h) const {
  if (not m_noReports) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
      if (tool->canReportAbout(h) and (not tool->isGood(elementId, ctx, h))) return false;
    }
  }
  return true;
}

bool
SCT_ConditionsSummaryTool::isGood(const IdentifierHash& elementHash) const {
  if (not m_noReports) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
      if (tool->canReportAbout(InDetConditions::SCT_SIDE) and (not tool->isGood(elementHash, ctx))) return false;
    }    
  }
  return true;
}

bool
SCT_ConditionsSummaryTool::isGood(const IdentifierHash& /*elementHash*/, const Identifier& elementId) const {
  if (not m_noReports) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
      if (tool->canReportAbout(InDetConditions::SCT_STRIP) and (not tool->isGood(elementId, ctx))) return false;
    } 
  }
  return true;
}

double 
SCT_ConditionsSummaryTool::goodFraction(const IdentifierHash& /*elementHash*/, const Identifier& /*idStart*/, const Identifier& /*idEnd*/) const {
  double result{1.0};
  ATH_MSG_WARNING("goodFraction is a deprecated function always returning 1.0 ");
  return result;
}
