#include "eflowRec/PFChargedFlowElementCreatorAlgorithm.h"

#include "xAODPFlow/FlowElementAuxContainer.h"

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

  SG::WriteHandle<xAOD::FlowElementContainer> chargedFlowElementContainerWriteHandle(m_chargedFlowElementContainerWriteHandleKey,ctx);
  ATH_CHECK(chargedFlowElementContainerWriteHandle.record(std::make_unique<xAOD::FlowElementContainer>(),std::make_unique<xAOD::FlowElementAuxContainer>()));
  
  /* Create Charged FlowElements from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(m_eflowCaloObjectContainerReadHandleKey,ctx);
  for (auto thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) createChargedFlowElements(*thisEflowCaloObject,true,chargedFlowElementContainerWriteHandle);

  std::sort(chargedFlowElementContainerWriteHandle->begin(), chargedFlowElementContainerWriteHandle->end(), [] (const xAOD::FlowElement* flowElement1, const xAOD::FlowElement* flowElement2) {return flowElement1->pt()>flowElement2->pt();});

  return StatusCode::SUCCESS;  

}

StatusCode PFChargedFlowElementCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS; }

void PFChargedFlowElementCreatorAlgorithm::createChargedFlowElements(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::FlowElementContainer>& chargedFlowElementContainerWriteHandle) const {

}
