/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

SCT_ChargeTrappingCondData SCT_RadDamageSummaryTool::getCondData(const IdentifierHash& elementHash, const double& zpos) const {
  return m_chargeTrappingTool->getCondData(elementHash, zpos);
}

void SCT_RadDamageSummaryTool::initPotentialValue() {
  m_chargeTrappingTool->getInitPotentialValue();
  return;
}

void SCT_RadDamageSummaryTool::holeTransport(double& x0, double& y0, double& xfin, double& yfin, double& Q_m2, double& Q_m1, double& Q_00, double& Q_p1, double& Q_p2) const {
  m_chargeTrappingTool->getHoleTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2);
  return;
}
