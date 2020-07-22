#include "eflowRec/PFNeutralFlowElementCreatorAlgorithm.h"

#include "xAODPFlow/FlowElementAuxContainer.h"

PFNeutralFlowElementCreatorAlgorithm::PFNeutralFlowElementCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode PFNeutralFlowElementCreatorAlgorithm::initialize(){

  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());
  ATH_CHECK(m_neutralFEContainerWriteHandleKey.initialize());
  if(!m_LCMode) {
    ATH_CHECK(m_neutralFEContainerWriteHandleKey_nonModified.initialize());
  }

   return StatusCode::SUCCESS;
}

StatusCode PFNeutralFlowElementCreatorAlgorithm::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG("Executing");

  /* Create Neutral PFOs from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(m_eflowCaloObjectContainerReadHandleKey,ctx);

  // Always create at least one FlowElement container & aux
  auto neutralFEContainer = std::make_unique<xAOD::FlowElementContainer>();
  auto neutralFEContainerAux = std::make_unique<xAOD::FlowElementAuxContainer>();
  neutralFEContainer->setStore(neutralFEContainerAux.get());
  // The non-modified container is only used for LC FlowElements
  std::unique_ptr<xAOD::FlowElementContainer> neutralFEContainer_nonModified(nullptr);
  std::unique_ptr<xAOD::FlowElementAuxContainer> neutralFEContainerAux_nonModified(nullptr);
  if(m_LCMode) {
    neutralFEContainer_nonModified = std::make_unique<xAOD::FlowElementContainer>();
    neutralFEContainerAux_nonModified = std::make_unique<xAOD::FlowElementAuxContainer>();
    neutralFEContainer->setStore(neutralFEContainerAux_nonModified.get());
  }

  ATH_MSG_DEBUG("Looping over eflowCaloObjects");
  // Create FlowElements and fill the containers
  for (auto thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) {
    if( createNeutralFlowElement(*thisEflowCaloObject, neutralFEContainer.get(), neutralFEContainer_nonModified.get()).isFailure() ) {
      ATH_MSG_WARNING("Problem encountered while creating neutral FlowElements");
      return StatusCode::SUCCESS;
    }
  }

  // Record the output containers
  SG::WriteHandle<xAOD::FlowElementContainer> neutralFEContainerWriteHandle(m_neutralFEContainerWriteHandleKey,ctx);
  std::sort(neutralFEContainer->begin(), neutralFEContainer->end(), [] (const xAOD::FlowElement* fe1, const xAOD::FlowElement* fe2) {return fe1->pt()>fe2->pt();});
  ATH_CHECK( neutralFEContainerWriteHandle.record(std::move(neutralFEContainer),std::move(neutralFEContainerAux)) );
  if(m_LCMode) {
    std::sort(neutralFEContainer_nonModified->begin(), neutralFEContainer_nonModified->end(), [] (const xAOD::FlowElement* fe1, const xAOD::FlowElement* fe2) {return fe1->pt()>fe2->pt();});
    SG::WriteHandle<xAOD::FlowElementContainer> neutralFEContainerWriteHandle_nonModified(m_neutralFEContainerWriteHandleKey,ctx);
    ATH_CHECK( neutralFEContainerWriteHandle_nonModified.record(std::move(neutralFEContainer_nonModified),std::move(neutralFEContainerAux_nonModified)) );
  }

  return StatusCode::SUCCESS;
}

StatusCode PFNeutralFlowElementCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS;}

StatusCode PFNeutralFlowElementCreatorAlgorithm::createNeutralFlowElement(const eflowCaloObject& energyFlowCaloObject, xAOD::FlowElementContainer* neutralFEContainer, xAOD::FlowElementContainer* neutralFEContainer_nonModified) const {
  return StatusCode::SUCCESS;
}