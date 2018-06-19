#include "TrigHLTJetRec/NegativeEnergyRejectionTool.h"


NegativeEnergyRejectionTool::NegativeEnergyRejectionTool(const std::string& name):
  AsgTool(name) {
}

StatusCode NegativeEnergyRejectionTool::initialize() {
  ATH_MSG_INFO("Initializing " << name() << ".");
  return StatusCode::SUCCESS; 
}


bool  NegativeEnergyRejectionTool::operator()(const xAOD::IParticle* ip) {
  ATH_MSG_DEBUG("Entering operator()()...");

  ++m_tested;
  
  bool null = (ip == 0);
  bool negativeE = m_skipNegativeEnergy && ip->e() <= 0.0;
  if  (null || negativeE){
    ++m_rejected;
    return true;
  }
  return false;
}


StatusCode NegativeEnergyRejectionTool::finalize() {
  ATH_MSG_INFO("Finalizing " << name() << ".");
  return StatusCode::SUCCESS; 
}


std::size_t NegativeEnergyRejectionTool::tested() const {return m_tested;}
std::size_t NegativeEnergyRejectionTool::rejected() const {return m_rejected;}
