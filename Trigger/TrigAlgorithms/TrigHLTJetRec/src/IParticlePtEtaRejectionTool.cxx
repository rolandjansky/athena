/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHLTJetRec/IParticlePtEtaRejectionTool.h"


IParticlePtEtaRejectionTool::IParticlePtEtaRejectionTool(const std::string& name): 
  AsgTool(name) {
  declareProperty("ptMin", m_ptMin);
  declareProperty("absEtaMax", m_absEtaMax);
}

//**********************************************************************

StatusCode IParticlePtEtaRejectionTool::initialize() {
  ATH_MSG_INFO("Initializing IParticlePtEtaRejectionTool "
               << name() << " minPt " << m_ptMin
               << " max abs eta "<<  m_absEtaMax);
  return StatusCode::SUCCESS;
}


StatusCode IParticlePtEtaRejectionTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


bool  IParticlePtEtaRejectionTool::operator()(const xAOD::IParticle* pj) {
  ATH_MSG_DEBUG("Entering operator()()...");

  ++m_tested;
  std::cout << "IParticlePtEtaRejectionTool::operator() "
            << " abs eta " << std::abs(pj->eta())
            << " pt " <<  pj -> pt()
            << '\n';
  if (std::abs(pj->eta()) > m_absEtaMax){
    ATH_MSG_DEBUG("rejecting ip with eta "<<  pj->eta() <<
                  " max linit: " << m_absEtaMax);
    ++m_rejected;
    return true;  // reject
  } 
  if (pj->pt() < m_ptMin){
    ATH_MSG_DEBUG("rejecting ip with pt "<< pj->pt() <<
                  " min linit: " << m_ptMin);
    ++m_rejected;
    return true; // reject
  }
  
  return false;
}

std::size_t IParticlePtEtaRejectionTool::tested() const {return m_tested;}
std::size_t IParticlePtEtaRejectionTool::rejected() const {return m_rejected;}

