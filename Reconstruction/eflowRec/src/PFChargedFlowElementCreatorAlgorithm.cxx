#include "eflowRec/PFChargedFlowElementCreatorAlgorithm.h"

PFChargedFlowElementCreatorAlgorithm::PFChargedFlowElementCreatorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name,pSvcLocator)
{
}

StatusCode PFChargedFlowElementCreatorAlgorithm::initialize(){

  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());

  ATH_CHECK(m_chargedFlowElementContainerWriteHandleKey.initialize());
  
  return StatusCode::SUCCESS;
}

StatusCode PFChargedFlowElementCreatorAlgorithm::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG("Starting PFOChargedCreatorAlgorithm::execute");

  return StatusCode::SUCCESS;  

}

StatusCode PFChargedFlowElementCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS; }

void PFChargedFlowElementCreatorAlgorithm::createChargedFlowElements(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::FlowElementContainer>& chargedFlowElementContainerWriteHandle) const {

}
