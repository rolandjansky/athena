#include "eflowRec/PFLCNeutralFlowElementCreatorAlgorithm.h"
#include "xAODCore/ShallowCopy.h"

PFLCNeutralFlowElementCreatorAlgorithm::PFLCNeutralFlowElementCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode PFLCNeutralFlowElementCreatorAlgorithm::initialize(){

  ATH_CHECK(m_neutralFEContainerReadHandleKey.initialize());
  ATH_CHECK(m_neutralFELCContainerWriteHandleKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PFLCNeutralFlowElementCreatorAlgorithm::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG("Executing");

  /* Create Neutral PFOs from all eflowCaloObjects */
  SG::ReadHandle<xAOD::FlowElementContainer> neutralFEContainerReadHandle(m_neutralFEContainerReadHandleKey,ctx);

  std::pair< xAOD::FlowElementContainer*, xAOD::ShallowAuxContainer* > shallowCopyPair = xAOD::shallowCopyContainer(*neutralFEContainerReadHandle);
  std::unique_ptr<xAOD::FlowElementContainer> neutralFELCContainer{shallowCopyPair.first};
  std::unique_ptr<xAOD::ShallowAuxContainer> neutralFELCContainerAux{shallowCopyPair.second};
  SG::WriteHandle<xAOD::FlowElementContainer> neutralFELCContainerWriteHandle(m_neutralFELCContainerWriteHandleKey,ctx);
  ATH_CHECK( neutralFELCContainerWriteHandle.record(std::move(neutralFELCContainer),std::move(neutralFELCContainerAux)));

  return StatusCode::SUCCESS;
}

StatusCode PFLCNeutralFlowElementCreatorAlgorithm::finalize(){
  return StatusCode::SUCCESS;
}
