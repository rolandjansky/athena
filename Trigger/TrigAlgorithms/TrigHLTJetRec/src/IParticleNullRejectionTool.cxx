/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHLTJetRec/IParticleNullRejectionTool.h"


IParticleNullRejectionTool::IParticleNullRejectionTool(const std::string& name): 
  AsgTool(name) {
}

//**********************************************************************

StatusCode IParticleNullRejectionTool::initialize() {
  ATH_MSG_INFO("Initializing IParticleNullRejectionTool " << name() << ".");
  return StatusCode::SUCCESS;
}


StatusCode IParticleNullRejectionTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


bool  IParticleNullRejectionTool::operator()(const xAOD::IParticle*) {
  ATH_MSG_DEBUG("Entering operator()()...");
  ++m_tested;
  return false;
}

std::size_t IParticleNullRejectionTool::tested() const {return m_tested;}
std::size_t IParticleNullRejectionTool::rejected() const {return 0;}

