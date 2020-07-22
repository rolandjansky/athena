#include "eflowRec/PFNeutralFlowElementCreatorAlgorithm.h"

#include "eflowRec/eflowRecCluster.h"

#include "xAODPFlow/FlowElementAuxContainer.h"

PFNeutralFlowElementCreatorAlgorithm::PFNeutralFlowElementCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode PFNeutralFlowElementCreatorAlgorithm::initialize(){

  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());
  ATH_CHECK(m_neutralFEContainerWriteHandleKey.initialize());

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

  ATH_MSG_DEBUG("Looping over eflowCaloObjects");
  // Create FlowElements and fill the containers
  for (auto thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) {
    if( createNeutralFlowElement(*thisEflowCaloObject, neutralFEContainer.get()).isFailure()) {
      ATH_MSG_WARNING("Problem encountered while creating neutral FlowElements");
      return StatusCode::SUCCESS;
    }
  }

  // Record the output containers
  SG::WriteHandle<xAOD::FlowElementContainer> neutralFEContainerWriteHandle(m_neutralFEContainerWriteHandleKey,ctx);
  std::sort(neutralFEContainer->begin(), neutralFEContainer->end(), [] (const xAOD::FlowElement* fe1, const xAOD::FlowElement* fe2) {return fe1->pt()>fe2->pt();});
  ATH_CHECK( neutralFEContainerWriteHandle.record(std::move(neutralFEContainer),std::move(neutralFEContainerAux)) );

  return StatusCode::SUCCESS;
}

StatusCode PFNeutralFlowElementCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS;}

StatusCode PFNeutralFlowElementCreatorAlgorithm::createNeutralFlowElement(const eflowCaloObject& energyFlowCaloObject, xAOD::FlowElementContainer* neutralFEContainer) const {

  unsigned int nClusters = energyFlowCaloObject.nClusters();

  ATH_MSG_VERBOSE("  eflowCaloObject with " << nClusters << " clusters");
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){
    eflowRecCluster* thisEfRecCluster = energyFlowCaloObject.efRecCluster(iCluster);
    
    /* Skip empty clusters (presumably subtraction remnants) */
    const CaloClusterCellLink* theCellLink = energyFlowCaloObject.efRecCluster(iCluster)->getCluster()->getCellLinks();
    CaloClusterCellLink::const_iterator it=theCellLink->begin();
    CaloClusterCellLink::const_iterator it_e=theCellLink->end();
    if (it == it_e) {
      continue;
    }
    //this vetoes rare examples where only two cells are left, and they have equal and opposite energy
    if (0.0 == energyFlowCaloObject.efRecCluster(iCluster)->getCluster()->e() ) continue;

    /* Create the FlowElement, add the cluster and set the four-momentum, charge and type */

    ATH_MSG_VERBOSE("Creating FlowElement");
    xAOD::FlowElement* thisFE = new xAOD::FlowElement();
    neutralFEContainer->push_back(thisFE);    

    ATH_MSG_VERBOSE("Get original cluster link");
    ElementLink<xAOD::CaloClusterContainer> theOriginalClusterLink = thisEfRecCluster->getOriginalClusElementLink();
    ATH_MSG_VERBOSE("Get sister cluster link");
    ElementLink<xAOD::CaloClusterContainer> theSisterClusterLink = (*theOriginalClusterLink)->getSisterClusterLink();

    std::vector<ElementLink<xAOD::IParticleContainer> > theClusters;
    ElementLink< xAOD::IParticleContainer > theIParticleTrackLink; 
    if (theSisterClusterLink.isValid()) theIParticleTrackLink.resetWithKeyAndIndex(theSisterClusterLink.persKey(),theSisterClusterLink.persIndex()); 
    else theIParticleTrackLink.resetWithKeyAndIndex(theOriginalClusterLink.persKey(),theOriginalClusterLink.persIndex()); 
    theClusters.push_back(theIParticleTrackLink);
    thisFE->setOtherObjectLinks(theClusters);

    if (true == m_addShowerSubtractedClusters){
      const SG::AuxElement::Accessor<ElementLink<xAOD::CaloClusterContainer> > accShowerSubtractedClusterLink("FEShowerSubtractedClusterLink");
      accShowerSubtractedClusterLink(*thisFE) = thisEfRecCluster->getClusElementLink();
    }

    ATH_MSG_VERBOSE("  Sucessfully set cluster link");

    /*
    const xAOD::CaloCluster* cluster = thisEfRecCluster->getCluster();
    ATH_MSG_VERBOSE("Got CaloCluster from EfRecCluster");
    //be careful here - cluster p4 methods do not store sign. Thus -ve energy clusters have +ve pt and hence +ve energy
    if (!m_LCMode) {
      //in EM->EM/LC mode we use eta,phi at EM scale for both 4-vectors
      thisFE->setP4(cluster->pt(), cluster->rawEta(), cluster->rawPhi(), cluster->m());
      thisFE->setP4EM(cluster->rawE()/cosh(cluster->rawEta()), cluster->rawEta(),cluster->rawPhi(),cluster->rawM());
    }
    else{
      //in LC-> mode we use the LC 4-vector for the LC scale
      thisPFO->setP4(cluster->pt(), cluster->eta(), cluster->phi(), cluster->m());
      //we cannot access geometric weights for LC clusters, so we make an approximation of the EM energy by looping over the calocells
      //Then the EM 4-vector uses the energy/pt at this EM scale + eta,phi from LC 4-vector
      const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
      float emPt = 0.0;
      for (auto thisCaloCell : *theCellLink) emPt += thisCaloCell->e()/cosh(thisCaloCell->eta());
      
      thisPFO->setP4EM(emPt,cluster->eta(),cluster->phi(),0.0);//mass is always zero at EM scale

    }
    */  

  }//cluster loop


  return StatusCode::SUCCESS;
}