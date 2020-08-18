#include "eflowRec/PFLCNeutralFlowElementCreatorAlgorithm.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

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

  for( unsigned int counter = 0; counter < (*neutralFEContainerReadHandle).size(); counter++){
    const xAOD::FlowElement* thisFE = (*neutralFEContainerReadHandle)[counter];

    const static SG::AuxElement::Accessor<ElementLink<xAOD::CaloClusterContainer> > accShowerSubtractedClusterLink("FEShowerSubtractedClusterLink");
    ElementLink<xAOD::CaloClusterContainer> clusElementLink = accShowerSubtractedClusterLink(*thisFE);

    xAOD::FlowElement* theCopiedFE = (*neutralFELCContainerWriteHandle)[counter];
    //be careful here - cluster p4 methods do not store sign. Thus -ve energy clusters have +ve pt and hence +ve energy
    //we use eta,phi at EM scale for both 4-vectors - standard FE are at EM scale
    theCopiedFE->setP4((*clusElementLink)->pt(), (*clusElementLink)->rawEta(), (*clusElementLink)->rawPhi(), (*clusElementLink)->m());
  }


  return StatusCode::SUCCESS;
}
