/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummaryTool.cxx
 * implementation file for SCT_ConditionsSummaryTool class
 * @author shaun.roe@cern.ch
 **/

#include "SCT_ConditionsSummaryTool.h"
#include "SCT_DetectorElementStatus.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

#include "GaudiKernel/EventContext.h"
#include "InDetIdentifier/SCT_ID.h"

// Constructor
SCT_ConditionsSummaryTool::SCT_ConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//Initialize
StatusCode 
SCT_ConditionsSummaryTool::initialize() {
  StatusCode sc{m_toolHandles.retrieve()};
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Retrieval of ToolHandleArray<ISCT_ConditionsTool> failed.");
    return sc;
  }
  ATH_CHECK( m_SCTDetEleCollKey.initialize() );
  // Get SCT helper
  ATH_CHECK(detStore()->retrieve(m_id_sct, "SCT_ID"));
  m_noReports = m_toolHandles.empty();
  return sc;
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

double
SCT_ConditionsSummaryTool::activeFraction(const IdentifierHash& elementHash, const Identifier& idStart, const Identifier& idEnd, const EventContext& ctx) const {
  return goodFraction(elementHash, idStart, idEnd, ctx);
}

bool 
SCT_ConditionsSummaryTool::isActive(const Identifier& elementId, const InDetConditions::Hierarchy h, const EventContext& ctx) const {
  return isGood(elementId, h, ctx);
}

bool 
SCT_ConditionsSummaryTool::isActive(const IdentifierHash& elementHash, const EventContext& ctx) const {
  return isGood(elementHash, ctx);
}

bool 
SCT_ConditionsSummaryTool::isActive(const IdentifierHash& elementHash, const Identifier& elementId, const EventContext& ctx) const {
  return isGood(elementHash, elementId, ctx);
}

bool
SCT_ConditionsSummaryTool::isGood(const Identifier& elementId, const InDetConditions::Hierarchy h) const {
  if (not m_noReports) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
       if (tool->canReportAbout(h) and (not tool->isGood(elementId, ctx, h))) {
          return false;
       }
    }
  }
  return true;
}

bool
SCT_ConditionsSummaryTool::isGood(const IdentifierHash& elementHash) const {
  if (not m_noReports) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
      if ((tool->canReportAbout(InDetConditions::SCT_SIDE) or
           tool->canReportAbout(InDetConditions::SCT_MODULE)) and
          (not tool->isGood(elementHash, ctx))) {
        return false;
      }
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

bool 
SCT_ConditionsSummaryTool::isGood(const Identifier& elementId, const InDetConditions::Hierarchy h, const EventContext& ctx) const {
  if (not m_noReports) {
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
      if (tool->canReportAbout(h) and (not tool->isGood(elementId, ctx, h))) return false;
    }
  }
  return true;
}

bool 
SCT_ConditionsSummaryTool::isGood(const IdentifierHash& elementHash, const EventContext& ctx) const {
  if (not m_noReports) {
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
      if ((tool->canReportAbout(InDetConditions::SCT_SIDE) or
           tool->canReportAbout(InDetConditions::SCT_MODULE)) and
          (not tool->isGood(elementHash, ctx))) {
        return false;
      }
    }    
  }
  return true;
}

std::unique_ptr<InDet::SiDetectorElementStatus> SCT_ConditionsSummaryTool::getDetectorElementStatus(const EventContext& ctx,
                                                                                                [[maybe_unused]] bool active_only) const {
   SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> condData{m_SCTDetEleCollKey, ctx};
   std::unique_ptr<InDet::SiDetectorElementStatus> element_status( new InDet::SCT_DetectorElementStatus(*(condData.cptr())) );
   if (not m_noReports) {
      for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
         // @TODO also check if it can report about chips ?
         if ((tool->canReportAbout(InDetConditions::SCT_SIDE) or
              tool->canReportAbout(InDetConditions::SCT_MODULE) or
              tool->canReportAbout(InDetConditions::SCT_STRIP))) {
            tool->getDetectorElementStatus(ctx,*element_status);
         }
      }
   }
   return element_status;
}

bool 
SCT_ConditionsSummaryTool::isGood(const IdentifierHash& /*elementHash*/, const Identifier& elementId, const EventContext& ctx) const {
  if (not m_noReports) {
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

double 
SCT_ConditionsSummaryTool::goodFraction(const IdentifierHash& /*elementHash*/, const Identifier& /*idStart*/, const Identifier& /*idEnd*/, const EventContext& /*ctx*/) const {
  double result{1.0};
  ATH_MSG_WARNING("goodFraction is a deprecated function always returning 1.0 ");
  return result;
}

bool
SCT_ConditionsSummaryTool::hasBSError(const IdentifierHash& /*elementHash*/) const {
  ATH_MSG_WARNING("hasBSError() is not implemented for SCT_ConditionsSummaryTool");
  return true;
}

bool
SCT_ConditionsSummaryTool::hasBSError(const IdentifierHash& /*elementHash*/, Identifier /*elementId*/) const {
  ATH_MSG_WARNING("hasBSError() is not implemented for SCT_ConditionsSummaryTool");
  return true;
}

bool
SCT_ConditionsSummaryTool::hasBSError(const IdentifierHash& /*elementHash*/, const EventContext& /*ctx*/) const {
  ATH_MSG_WARNING("hasBSError() is not implemented for SCT_ConditionsSummaryTool");
  return true;
}

bool
SCT_ConditionsSummaryTool::hasBSError(const IdentifierHash& /*elementHash*/, Identifier /*elementId*/, const EventContext& /*ctx*/) const {
  ATH_MSG_WARNING("hasBSError() is not implemented for SCT_ConditionsSummaryTool");
  return true;
}

bool
SCT_ConditionsSummaryTool::hasBSError(const IdentifierHash& /*elementHash*/, const EventContext& /*ctx*/, Identifier /*elementId*/) const {
  ATH_MSG_WARNING("hasBSError() is not implemented for SCT_ConditionsSummaryTool");
  return true;
}


uint64_t
SCT_ConditionsSummaryTool::getBSErrorWord(const IdentifierHash& /*moduleHash*/, const EventContext& /*ctx*/) const {
  ATH_MSG_WARNING("getBSErrorWord() is not implemented for SCT_ConditionsSummaryTool");
  return 0;
}

uint64_t
SCT_ConditionsSummaryTool::getBSErrorWord(const IdentifierHash& /*moduleHash*/, const int /*index*/, const EventContext& /*ctx*/) const {
  ATH_MSG_WARNING("getBSErrorWord() is not implemented for SCT_ConditionsSummaryTool");
  return 0;
}

