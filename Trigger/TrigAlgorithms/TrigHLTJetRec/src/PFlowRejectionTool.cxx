#include "TrigHLTJetRec/PFlowRejectionTool.h"
#include "xAODPFlow/PFO.h"

PFlowRejectionTool::PFlowRejectionTool(const std::string& name):
  AsgTool(name) {
  declareProperty("skipNegativeEnergy", m_skipNegativeEnergy);

}

StatusCode PFlowRejectionTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}

bool PFlowRejectionTool::operator()(const xAOD::IParticle* ip) {

  ++m_tested;
  const xAOD::PFO* pfo = dynamic_cast<const xAOD::PFO*>(ip);
      
  // keep charged PFOs with energy==0 because for MET TST with PFlow, 
  // there may be high pt 
  // charged PFOs that receive a weight of 0 due to being in dense 
  // showers, but need to be present for overlap removal, because they 
  // don't retain these weights when added to the TST      
  
  if( fabs(pfo->charge())>FLT_MIN) {
    const static SG::AuxElement::ConstAccessor<char>
      PVMatchedAcc("matchedToPV");
    if (!PVMatchedAcc(*pfo)){
      ++m_rejected;
      return true;
    }
  }
  
  // This also skips 0 energy, which has no effect other than
  // on memory size, but is used in some workflows for pileup rejection
  if(m_skipNegativeEnergy && pfo->e()<FLT_MIN){
    ++m_rejected;
    return true;
  }
  
  return false;
}


StatusCode PFlowRejectionTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

std::size_t PFlowRejectionTool::tested() const {return m_tested;}
std::size_t PFlowRejectionTool::rejected() const {return m_rejected;}


