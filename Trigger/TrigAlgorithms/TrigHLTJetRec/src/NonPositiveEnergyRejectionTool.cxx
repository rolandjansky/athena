/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigHLTJetRec/NonPositiveEnergyRejectionTool.h"


NonPositiveEnergyRejectionTool::NonPositiveEnergyRejectionTool(const std::string& name):
  AsgTool(name) {
}

StatusCode NonPositiveEnergyRejectionTool::initialize() {
  ATH_MSG_INFO("Initializing " << name() << ".");
  return StatusCode::SUCCESS; 
}


bool  NonPositiveEnergyRejectionTool::operator()(const xAOD::IParticle* ip) {
  ATH_MSG_DEBUG("Entering operator()()...");

  ++m_tested;
  
  bool null = (ip == 0);
  bool positiveE = ip->e() > FLT_MIN;
  if  (null || !positiveE){
    ++m_rejected;
    return true;
  }
  return false;
}


StatusCode NonPositiveEnergyRejectionTool::finalize() {
  ATH_MSG_INFO("Finalizing " << name() << ".");
  return StatusCode::SUCCESS; 
}


std::size_t NonPositiveEnergyRejectionTool::tested() const {return m_tested;}
std::size_t NonPositiveEnergyRejectionTool::rejected() const {return m_rejected;}
