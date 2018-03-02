/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RadDamageSummaryTool.h"

#include "Identifier/IdentifierHash.h"

// -- constructor
SCT_RadDamageSummaryTool::SCT_RadDamageSummaryTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent),
  m_useTrapping(true)
  {
    declareProperty("UseChargeTrappingTool",m_useTrapping, "Set to true to switch on charge trapping");
    declareProperty("DoChargeTrapping",m_doCTrap=false, "Do charge trapping");
  }

//-- initialize
StatusCode SCT_RadDamageSummaryTool::initialize() {
  ATH_MSG_INFO("Initializing SCT_RadDamageSummaryTool");
  
  StatusCode sc = setProperties();
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL("Unable to set properties");
    return StatusCode::FAILURE;
  }
  
  if (m_useTrapping) {
    if (StatusCode::SUCCESS!=m_chargeTrappingTool.retrieve()) {
      ATH_MSG_FATAL("Unable to retrieve SCT_ChargeTrappingTool");
      return StatusCode::FAILURE;
    } 
    ATH_MSG_INFO("SCT_ChargeTrappingTool retrieved");
  } else {
    m_chargeTrappingTool.disable();
  }
  
  return StatusCode::SUCCESS; 
}


// -- finalize
StatusCode SCT_RadDamageSummaryTool::finalize() {
  ATH_MSG_INFO("Finalizing SCT_RadDamageSummaryTool");
  
  return StatusCode::SUCCESS;
}

bool SCT_RadDamageSummaryTool::doCTrap(const IdentifierHash & elementHash, const double & zpos) {
  m_doCTrap = false;
  if (m_useTrapping) {
    m_doCTrap =  m_chargeTrappingTool->getdoCTrap(elementHash, zpos);
  }
  return m_doCTrap;
}


// -- Charge trapping probability
double SCT_RadDamageSummaryTool::ChargeTrappingProbability(const IdentifierHash & elementHash, const double & zpos) {
  double prob = 0.0;
  if (m_useTrapping) {
    prob = m_chargeTrappingTool->getTrappingProbability(elementHash, zpos);
  }
  return prob;
}

// -- Trapping Constant
double SCT_RadDamageSummaryTool::TrappingConstant(const IdentifierHash & elementHash, const double & zpos) {
  double k = 0.0;
  if (m_useTrapping) {
    k = m_chargeTrappingTool->getTrappingHoles(elementHash, zpos);
  }
  return k;
}


// -- Electric Field
double SCT_RadDamageSummaryTool::ElectricField(const IdentifierHash & elementHash, const double & zpos) {
  double k = 0.0;
  if (m_useTrapping) {
    k = m_chargeTrappingTool->getElectricField(elementHash, zpos);
  }
  return k;
}

// -- Trapping time: Time at which the trapping happened
double SCT_RadDamageSummaryTool::TrappingTime(const IdentifierHash & elementHash, const double & zpos) {
  double k = 0.0;
  if (m_useTrapping) {
    k = m_chargeTrappingTool->getTrappingTime(elementHash, zpos);
  }
  return k;
}

// -- Time to electrode: Time that the charge needs to arrive to the electrode
double SCT_RadDamageSummaryTool::TimeToElectrode(const IdentifierHash & elementHash, const double & zpos) {
  double k = 0.0;
  if (m_useTrapping) {
    k = m_chargeTrappingTool->getTimeToElectrode(elementHash, zpos);
  }
  return k;
}

// -- Trapping position: position at which the trapping happened
double SCT_RadDamageSummaryTool::TrappingPositionZ(const IdentifierHash & elementHash, const double & zpos) {
  double k = 0.0;
  if (m_useTrapping) {
    k = m_chargeTrappingTool->getTrappingPositionZ(elementHash, zpos);
  }
  return k;
}

// -- Hole drift mobility
double SCT_RadDamageSummaryTool::HoleDriftMobility(const IdentifierHash & elementHash, const double & zpos) {
  double k = 0.0;
  if (m_useTrapping) {
    k = m_chargeTrappingTool->getHoleDriftMobility(elementHash, zpos);
  }
  return k;
}

//------------------------------------------------
// -- Ramo Potential
//-----------------------------------------------
void SCT_RadDamageSummaryTool::InitPotentialValue() {
  if (m_useTrapping) {
    m_chargeTrappingTool->getInitPotentialValue(); 
  }
  return;
}

void SCT_RadDamageSummaryTool::HoleTransport(double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 ) const {  
  if (m_useTrapping) {
    m_chargeTrappingTool->getHoleTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2 );
  }
  return;
}
