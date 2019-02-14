/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RadDamageSummaryTool.h"

#include "Identifier/IdentifierHash.h"

// -- constructor
SCT_RadDamageSummaryTool::SCT_RadDamageSummaryTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent)
{
}

//-- initialize
StatusCode SCT_RadDamageSummaryTool::initialize() {
  ATH_MSG_INFO("Initializing SCT_RadDamageSummaryTool");

  ATH_CHECK(setProperties());
  
  ATH_CHECK(m_chargeTrappingTool.retrieve());
  ATH_MSG_INFO("SCT_ChargeTrappingTool retrieved");
  
  return StatusCode::SUCCESS;
}

// -- finalize
StatusCode SCT_RadDamageSummaryTool::finalize() {
  ATH_MSG_INFO("Finalizing SCT_RadDamageSummaryTool");
  
  return StatusCode::SUCCESS;
}

SCT_ChargeTrappingCondData SCT_RadDamageSummaryTool::getCondData(const IdentifierHash& elementHash, const double& zpos, const EventContext& ctx) const {
  return m_chargeTrappingTool->getCondData(elementHash, zpos, ctx);
}

SCT_ChargeTrappingCondData SCT_RadDamageSummaryTool::getCondData(const IdentifierHash& elementHash, const double& zpos) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getCondData(elementHash, zpos, ctx);
}

void SCT_RadDamageSummaryTool::holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2, const EventContext& ctx) const {
  m_chargeTrappingTool->getHoleTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2, ctx);
}

void SCT_RadDamageSummaryTool::holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  holeTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2, ctx);
}
