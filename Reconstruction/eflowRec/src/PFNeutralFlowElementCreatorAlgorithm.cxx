#include "eflowRec/PFNeutralFlowElementCreatorAlgorithm.h"
#include "eflowRec/eflowRecCluster.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODPFlow/FlowElementAuxContainer.h"
#include "xAODPFlow/FEHelpers.h"

StatusCode PFNeutralFlowElementCreatorAlgorithm::initialize(){

  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());
  ATH_CHECK(m_neutralFEContainerWriteHandleKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PFNeutralFlowElementCreatorAlgorithm::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG("Executing");

  /* Create Neutral PFOs from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(
    m_eflowCaloObjectContainerReadHandleKey, ctx);

  // Always create at least one FlowElement container & aux
  auto neutralFEContainer = std::make_unique<xAOD::FlowElementContainer>();
  auto neutralFEContainerAux = std::make_unique<xAOD::FlowElementAuxContainer>();
  neutralFEContainer->setStore(neutralFEContainerAux.get());

  ATH_MSG_DEBUG("Looping over eflowCaloObjects");
  // Create FlowElements and fill the containers
  for (const auto *thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) {
    if( createNeutralFlowElement(*thisEflowCaloObject, neutralFEContainer.get()).isFailure()) {
      ATH_MSG_WARNING("Problem encountered while creating neutral FlowElements");
      return StatusCode::SUCCESS;
    }
  }

  // Record the output containers
  SG::WriteHandle<xAOD::FlowElementContainer> neutralFEContainerWriteHandle(
    m_neutralFEContainerWriteHandleKey, ctx);
  std::sort(neutralFEContainer->begin(),
            neutralFEContainer->end(),
            [](const xAOD::FlowElement* fe1, const xAOD::FlowElement* fe2) {
              return fe1->pt() > fe2->pt();
            });
  ATH_CHECK(neutralFEContainerWriteHandle.record(
    std::move(neutralFEContainer), std::move(neutralFEContainerAux)));

  return StatusCode::SUCCESS;
}

StatusCode
PFNeutralFlowElementCreatorAlgorithm::createNeutralFlowElement(
  const eflowCaloObject& energyFlowCaloObject,
  xAOD::FlowElementContainer* neutralFEContainer) const
{

  for (unsigned int iCluster = 0; iCluster < energyFlowCaloObject.nClusters(); ++iCluster){
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
    
    ATH_MSG_VERBOSE("Cluster link valid? "<<theOriginalClusterLink.isValid()<<"");
    ATH_MSG_VERBOSE("Get sister cluster link");
    ElementLink<xAOD::CaloClusterContainer> theSisterClusterLink = (*theOriginalClusterLink)->getSisterClusterLink();
    ATH_MSG_VERBOSE("Sister cluster link valid? "<<theSisterClusterLink.isValid()<<"");

    std::vector<ElementLink<xAOD::IParticleContainer>> theClusters;
    if (theSisterClusterLink.isValid()){
      theClusters.emplace_back(ElementLink<xAOD::IParticleContainer>(theSisterClusterLink));
    }
    else{
      theClusters.emplace_back(ElementLink<xAOD::IParticleContainer>(theOriginalClusterLink));
    }
    thisFE->setOtherObjectLinks(theClusters);

    const static SG::AuxElement::Accessor<
      ElementLink<xAOD::CaloClusterContainer>>
      accShowerSubtractedClusterLink("FEShowerSubtractedClusterLink");
    accShowerSubtractedClusterLink(*thisFE) =
      thisEfRecCluster->getClusElementLink();

    ATH_MSG_VERBOSE("  Sucessfully set cluster link");

    const xAOD::CaloCluster* cluster = thisEfRecCluster->getCluster();
    ATH_MSG_VERBOSE("Got CaloCluster from EfRecCluster");
    // be careful here - cluster p4 methods do not store sign. Thus -ve energy
    // clusters have +ve pt and hence +ve energy we use eta,phi at EM scale for
    // both 4-vectors - standard FE are at EM scale
    thisFE->setP4(cluster->rawE() / cosh(cluster->rawEta()),
                  cluster->rawEta(),
                  cluster->rawPhi(),
                  cluster->rawM());

    ATH_MSG_DEBUG("Created neutral FlowElement with E, pt, eta and phi of "
                  << thisFE->e() << ", " << thisFE->pt() << ", "
                  << thisFE->eta() << " and " << thisFE->phi());

    thisFE->setCharge(0);
    thisFE->setSignalType(xAOD::FlowElement::NeutralPFlow);

    FEHelpers::FillNeutralFlowElements FEFiller;
    FEFiller.addStandardMoments(*thisFE,*cluster);

    if (m_useCalibHitTruth) FEFiller.addStandardCalHitMoments(*thisFE,*cluster);

    FEFiller.addStandardSamplingEnergies(*thisFE,*cluster);
    
    float layerEnergy_TileBar0 = cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0);
    float layerEnergy_TileExt0 = cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0);
    const static SG::AuxElement::Accessor<float> accFloatTIle0E("LAYERENERGY_TILE0");
    accFloatTIle0E(*thisFE) = layerEnergy_TileBar0 + layerEnergy_TileExt0;

    const static SG::AuxElement::Accessor<float> accFloatTiming("TIMING");
    accFloatTiming(*thisFE) = cluster->time();
 
  }//cluster loop
  return StatusCode::SUCCESS;
}