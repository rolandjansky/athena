#include "TrigHLTJetRec/EMTopoRejectionTool.h"
#include "xAODCaloEvent/CaloCluster.h"

EMTopoRejectionTool::EMTopoRejectionTool(const std::string& name):
  AsgTool(name) {
}

StatusCode EMTopoRejectionTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}

bool EMTopoRejectionTool::operator()(const xAOD::IParticle* ip) {
  ATH_MSG_DEBUG ("operator()()...");

  ++m_tested;
  
  if(ip->e() <= 0.0){
    ++m_rejected;
    return true;
  }
  
  auto cluster = dynamic_cast<const xAOD::CaloCluster*>(ip);
  if (cluster == 0) { // reject if not a cluster
    ++m_rejected; 
    return true;
  }

  return false;
}

std::size_t EMTopoRejectionTool::tested() const {return m_tested;}
std::size_t EMTopoRejectionTool::rejected() const {return m_rejected;}


StatusCode EMTopoRejectionTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}



