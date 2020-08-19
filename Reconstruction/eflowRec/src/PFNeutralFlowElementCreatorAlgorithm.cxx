#include "eflowRec/PFNeutralFlowElementCreatorAlgorithm.h"

#include "eflowRec/eflowRecCluster.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODPFlow/FlowElementAuxContainer.h"

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

StatusCode PFNeutralFlowElementCreatorAlgorithm::createNeutralFlowElement(const eflowCaloObject& energyFlowCaloObject, xAOD::FlowElementContainer* neutralFEContainer) const {

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
    ATH_MSG_VERBOSE("Get sister cluster link");
    ElementLink<xAOD::CaloClusterContainer> theSisterClusterLink = (*theOriginalClusterLink)->getSisterClusterLink();

    std::vector<ElementLink<xAOD::IParticleContainer> > theClusters;
    ElementLink< xAOD::IParticleContainer > theIParticleTrackLink; 
    if (theSisterClusterLink.isValid()) theIParticleTrackLink.resetWithKeyAndIndex(theSisterClusterLink.persKey(),theSisterClusterLink.persIndex()); 
    else theIParticleTrackLink.resetWithKeyAndIndex(theOriginalClusterLink.persKey(),theOriginalClusterLink.persIndex()); 
    theClusters.push_back(theIParticleTrackLink);
    thisFE->setOtherObjectLinks(theClusters);

    const static SG::AuxElement::Accessor<ElementLink<xAOD::CaloClusterContainer> > accShowerSubtractedClusterLink("FEShowerSubtractedClusterLink");
    accShowerSubtractedClusterLink(*thisFE) = thisEfRecCluster->getClusElementLink();

    ATH_MSG_VERBOSE("  Sucessfully set cluster link");

    const xAOD::CaloCluster* cluster = thisEfRecCluster->getCluster();
    ATH_MSG_VERBOSE("Got CaloCluster from EfRecCluster");
    //be careful here - cluster p4 methods do not store sign. Thus -ve energy clusters have +ve pt and hence +ve energy
    //we use eta,phi at EM scale for both 4-vectors - standard FE are at EM scale
    thisFE->setP4(cluster->rawE()/cosh(cluster->rawEta()), cluster->rawEta(),cluster->rawPhi(),cluster->rawM());
    
    ATH_MSG_DEBUG("Created neutral FlowElement with E, pt, eta and phi of " << thisFE->e() << ", " << thisFE->pt() << ", " << thisFE->eta() << " and " << thisFE->phi());
    
    thisFE->setCharge(0);

    this->addMoment(xAOD::CaloCluster::CENTER_MAG,"PF_CENTER_MAG",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::SECOND_R,"PF_SECOND_R",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::CENTER_LAMBDA,"PF_CENTER_LAMBDA",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::ENG_BAD_CELLS,"PF_ENG_BAD_CELLS",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::N_BAD_CELLS,"PF_N_BAD_CELLS",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::BADLARQ_FRAC,"PF_BADLARQ_FRAC",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::ENG_POS,"PF_ENG_POS",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::AVG_LAR_Q,"PF_AVG_LAR_Q",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::AVG_TILE_Q,"PF_AVG_TILE_Q",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::ISOLATION,"PF_ISOLATION",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::SECOND_LAMBDA,"PF_SECOND_LAMBDA",*cluster,*thisFE);
    this->addMoment(xAOD::CaloCluster::EM_PROBABILITY,"PF_EM_PROBABILITY",*cluster,*thisFE);

    if (m_useCalibHitTruth){
      this->addMoment(xAOD::CaloCluster::ENG_CALIB_TOT,"PF_ENG_CALIB_TOT",*cluster,*thisFE);
      this->addMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_EM,"PF_ENG_CALIB_FRAC_EM",*cluster,*thisFE);
      this->addMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_HAD,"PF_ENG_CALIB_FRAC_HAD",*cluster,*thisFE);
      this->addMoment(xAOD::CaloCluster::ENG_CALIB_FRAC_REST,"PF_ENG_CALIB_FRAC_REST",*cluster,*thisFE);
    }

    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::PreSamplerB,"PF_LAYERENERGY_PreSamplerB",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::EMB1,"PF_LAYERENERGY_EMB1",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::EMB2,"PF_LAYERENERGY_EMB2",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::EMB3,"PF_LAYERENERGY_EMB3",*cluster,*thisFE);
    
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::PreSamplerE,"PF_LAYERENERGY_PreSamplerE",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::EME1,"PF_LAYERENERGY_EME1",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::EME2,"PF_LAYERENERGY_EME2",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::EME3,"PF_LAYERENERGY_EME3",*cluster,*thisFE);

    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::HEC0,"PF_LAYERENERGY_HEC0",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::HEC1,"PF_LAYERENERGY_HEC1",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::HEC2,"PF_LAYERENERGY_HEC2",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::HEC3,"PF_LAYERENERGY_HEC3",*cluster,*thisFE);

    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileBar0,"PF_LAYERENERGY_TileBar0",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileBar1,"PF_LAYERENERGY_TileBar1",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileBar2,"PF_LAYERENERGY_TileBar2",*cluster,*thisFE);

    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileGap1,"PF_LAYERENERGY_TileGap1",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileGap2,"PF_LAYERENERGY_TileGap2",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileGap3,"PF_LAYERENERGY_TileGap3",*cluster,*thisFE);

    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileExt0,"PF_LAYERENERGY_TileExt0",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileExt1,"PF_LAYERENERGY_TileExt1",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::TileExt2,"PF_LAYERENERGY_TileExt2",*cluster,*thisFE);

    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::FCAL0,"PF_LAYERENERGY_FCAL0",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::FCAL1,"PF_LAYERENERGY_FCAL1",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::FCAL2,"PF_LAYERENERGY_FCAL2",*cluster,*thisFE);

    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::MINIFCAL0,"PF_LAYERENERGY_MINIFCAL0",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::MINIFCAL1,"PF_LAYERENERGY_MINIFCAL1",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::MINIFCAL2,"PF_LAYERENERGY_MINIFCAL2",*cluster,*thisFE);
    this->addSamplingEnergy(xAOD::CaloCluster::CaloSample::MINIFCAL3,"PF_LAYERENERGY_MINIFCAL3",*cluster,*thisFE);

    float layerEnergy_TileBar0 = cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0);
    float layerEnergy_TileExt0 = cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0);
    const static SG::AuxElement::Accessor<float> accFloatTIle0E("PF_LAYERENERGY_TILE0");
    accFloatTIle0E(*thisFE) = layerEnergy_TileBar0 + layerEnergy_TileExt0;

    const static SG::AuxElement::Accessor<float> accFloatTiming("PF_TIMING");
    accFloatTiming(*thisFE) = cluster->time();
 
  }//cluster loop
  return StatusCode::SUCCESS;
}

void PFNeutralFlowElementCreatorAlgorithm::addMoment(const xAOD::CaloCluster::MomentType& momentType, const std::string& feAttribute, const xAOD::CaloCluster& theCluster, xAOD::FlowElement& theFE) const {

  double moment = 0.0;
  bool isRetrieved = theCluster.retrieveMoment(momentType, moment);
  if (true == isRetrieved) {    
    float float_moment = static_cast<float>(moment);
    const static SG::AuxElement::Accessor<float> accFloat(feAttribute);
    accFloat(theFE) = float_moment;
  }
  else ATH_MSG_WARNING(" Could not retrieve moment from the CaloCluster");

}

void PFNeutralFlowElementCreatorAlgorithm::addSamplingEnergy(const xAOD::CaloCluster::CaloSample& sampling, const std::string& feAttribute, const xAOD::CaloCluster& theCluster, xAOD::FlowElement& theFE) const {
  const static SG::AuxElement::Accessor<float> accFloat(feAttribute);
  accFloat(theFE) = theCluster.eSample(sampling);
}
