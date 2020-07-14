#include "eflowRec/PFNeutralFlowElementCreatorAlgorithm.h"

PFNeutralFlowElementCreatorAlgorithm::PFNeutralFlowElementCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PFNeutralFlowElementCreatorAlgorithm::initialize(){
   return StatusCode::SUCCESS;
}

StatusCode PFNeutralFlowElementCreatorAlgorithm::execute(const EventContext& ctx) const {
  return StatusCode::SUCCESS;
}

StatusCode PFNeutralFlowElementCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS;}

StatusCode PFNeutralFlowElementCreatorAlgorithm::createNeutralFlowElement(const eflowCaloObject& energyFlowCaloObject, xAOD::FlowElementContainer* neutralFEContainer, xAOD::FlowElementContainer* neutralFEContainer_nonModified) const {
  return StatusCode::SUCCESS;
}