#include "eflowRec/PFONeutralCreatorAlgorithm.h"

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"

#include "xAODPFlow/PFOAuxContainer.h"

PFONeutralCreatorAlgorithm::PFONeutralCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
{
}

StatusCode PFONeutralCreatorAlgorithm::initialize(){

  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());
  ATH_CHECK(m_neutralPFOContainerWriteHandleKey.initialize());
  ATH_CHECK(m_neutralPFOContainerWriteHandleKey_nonModified.initialize());
  return StatusCode::SUCCESS;
  
}

StatusCode PFONeutralCreatorAlgorithm::execute(){

  ATH_MSG_DEBUG("Executing");

  SG::WriteHandle<xAOD::PFOContainer> neutralPFOContainerWriteHandle(m_neutralPFOContainerWriteHandleKey);
  ATH_CHECK(neutralPFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(),std::make_unique<xAOD::PFOAuxContainer>()));
  SG::WriteHandle<xAOD::PFOContainer> neutralPFOContainerWriteHandle_nonModified(m_neutralPFOContainerWriteHandleKey_nonModified);
  if (m_LCMode) ATH_CHECK(neutralPFOContainerWriteHandle_nonModified.record(std::make_unique<xAOD::PFOContainer>(),std::make_unique<xAOD::PFOAuxContainer>()));
    
  /* Create Neutral PFOs from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(m_eflowCaloObjectContainerReadHandleKey);
  for (auto thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) createNeutralPFO(*thisEflowCaloObject, neutralPFOContainerWriteHandle, neutralPFOContainerWriteHandle_nonModified);

  return StatusCode::SUCCESS;
}

StatusCode PFONeutralCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS; }

void PFONeutralCreatorAlgorithm::createNeutralPFO(const eflowCaloObject& energyFlowCaloObject,SG::WriteHandle<xAOD::PFOContainer>& neutralPFOContainerWriteHandle, SG::WriteHandle<xAOD::PFOContainer>& neutralPFOContainerWriteHandle_nonModified){

  unsigned int nClusters = energyFlowCaloObject.nClusters();

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

    /* Create the efo, add the cluster and set the four-momentum, charge and type */

    xAOD::PFO* thisPFO = new xAOD::PFO();
    if (m_LCMode) {
      if (thisEfRecCluster->isTouchable()) {
        neutralPFOContainerWriteHandle->push_back(thisPFO);
      } else {
	neutralPFOContainerWriteHandle_nonModified->push_back(thisPFO);
      }
    } else {
      neutralPFOContainerWriteHandle->push_back(thisPFO);
    }

    ElementLink<xAOD::CaloClusterContainer> theOriginalClusterLink = thisEfRecCluster->getOriginalClusElementLink();
    bool isSet = thisPFO->setClusterLink(theOriginalClusterLink);
    if (!isSet) { msg(MSG::WARNING) << "Could not set Cluster in PFO " << endmsg; }

    const xAOD::CaloCluster* cluster = thisEfRecCluster->getCluster();
    //be careful here - cluster p4 methods do not store sign. Thus -ve energy clusters have +ve pt and hence +ve energy
    if (!m_LCMode) {
      //in EM->EM/LC mode we use eta,phi at EM scale for both 4-vectors
      thisPFO->setP4(cluster->pt(), cluster->rawEta(), cluster->rawPhi(), cluster->m());
      thisPFO->setP4EM(cluster->rawE()/cosh(cluster->rawEta()), cluster->rawEta(),cluster->rawPhi(),cluster->rawM());
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

    ATH_MSG_DEBUG("Created neutral PFO with E, pt, eta and phi of " << thisPFO->e() << ", " << thisPFO->pt() << ", " << thisPFO->eta() << " and " << thisPFO->phi());
    
    thisPFO->setCharge(0);

    //Set the CENTER_MAG moment. This has its own dedicated getter in the PFO EDM, and so is not dealt with by the generic addMoment function.
    double center_mag = 0.0;
    bool isRetrieved = cluster->retrieveMoment(xAOD::CaloCluster::CENTER_MAG, center_mag );
    if (true == isRetrieved) thisPFO->setCenterMag(center_mag);
    else ATH_MSG_WARNING("Could not retreve CENTER_MAG from xAOD::CaloCluster");
    
    //now set the moments for touchable clusters (i.e. ones we modify) in LC mode or all clusters in EM mode
    if ( (m_LCMode && thisEfRecCluster->isTouchable()) || !m_LCMode) {
      this->addMoment(xAOD::CaloCluster::SECOND_R,xAOD::PFODetails::PFOAttributes::eflowRec_SECOND_R,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::CENTER_LAMBDA,xAOD::PFODetails::PFOAttributes::eflowRec_CENTER_LAMBDA,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::ENG_BAD_CELLS,xAOD::PFODetails::PFOAttributes::eflowRec_ENG_BAD_CELLS,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::N_BAD_CELLS,xAOD::PFODetails::PFOAttributes::eflowRec_N_BAD_CELLS,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::BADLARQ_FRAC,xAOD::PFODetails::PFOAttributes::eflowRec_BADLARQ_FRAC,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::ENG_POS,xAOD::PFODetails::PFOAttributes::eflowRec_ENG_POS,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::AVG_LAR_Q,xAOD::PFODetails::PFOAttributes::eflowRec_AVG_LAR_Q,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::AVG_TILE_Q,xAOD::PFODetails::PFOAttributes::eflowRec_AVG_TILE_Q,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::ISOLATION,xAOD::PFODetails::PFOAttributes::eflowRec_ISOLATION,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::SECOND_LAMBDA,xAOD::PFODetails::PFOAttributes::eflowRec_SECOND_LAMBDA,*cluster, *thisPFO);
      this->addMoment(xAOD::CaloCluster::EM_PROBABILITY,xAOD::PFODetails::PFOAttributes::eflowRec_EM_PROBABILITY,*cluster, *thisPFO);
    }

    //First set all the layer energies
    float layerEnergy_preSamplerB = cluster->eSample(xAOD::CaloCluster::CaloSample::PreSamplerB);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_PreSamplerB = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_PreSamplerB;
    thisPFO->setAttribute( myAttribute_layerEnergy_PreSamplerB, layerEnergy_preSamplerB);

    float layerEnergy_EMB1 = cluster->eSample(xAOD::CaloCluster::CaloSample::EMB1);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EMB1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB1;
    thisPFO->setAttribute( myAttribute_layerEnergy_EMB1, layerEnergy_EMB1);

    float layerEnergy_EMB2 = cluster->eSample(xAOD::CaloCluster::CaloSample::EMB2);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EMB2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB2;
    thisPFO->setAttribute( myAttribute_layerEnergy_EMB2, layerEnergy_EMB2);

    float layerEnergy_EMB3 = cluster->eSample(xAOD::CaloCluster::CaloSample::EMB3);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EMB3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EMB3;
    thisPFO->setAttribute( myAttribute_layerEnergy_EMB3, layerEnergy_EMB3);

    float layerEnergy_preSamplerE = cluster->eSample(xAOD::CaloCluster::CaloSample::PreSamplerE);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_PreSamplerE = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_PreSamplerE;
    thisPFO->setAttribute( myAttribute_layerEnergy_PreSamplerE, layerEnergy_preSamplerE);

    float layerEnergy_EME1 = cluster->eSample(xAOD::CaloCluster::CaloSample::EME1);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EME1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME1;
    thisPFO->setAttribute( myAttribute_layerEnergy_EME1, layerEnergy_EME1);

    float layerEnergy_EME2 = cluster->eSample(xAOD::CaloCluster::CaloSample::EME2);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EME2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME2;
    thisPFO->setAttribute( myAttribute_layerEnergy_EME2, layerEnergy_EME2);

    float layerEnergy_EME3 = cluster->eSample(xAOD::CaloCluster::CaloSample::EME3);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EME3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EME3;
    thisPFO->setAttribute( myAttribute_layerEnergy_EME3, layerEnergy_EME3);

    float layerEnergy_HEC0 = cluster->eSample(xAOD::CaloCluster::CaloSample::HEC0);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC0;
    thisPFO->setAttribute( myAttribute_layerEnergy_HEC0, layerEnergy_HEC0);

    float layerEnergy_HEC1 = cluster->eSample(xAOD::CaloCluster::CaloSample::HEC1);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC1;
    thisPFO->setAttribute( myAttribute_layerEnergy_HEC1, layerEnergy_HEC1);

    float layerEnergy_HEC2 = cluster->eSample(xAOD::CaloCluster::CaloSample::HEC2);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC2;
    thisPFO->setAttribute( myAttribute_layerEnergy_HEC2, layerEnergy_HEC2);

    float layerEnergy_HEC3 = cluster->eSample(xAOD::CaloCluster::CaloSample::HEC3);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC3;
    thisPFO->setAttribute( myAttribute_layerEnergy_HEC3, layerEnergy_HEC3);

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileBar0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar0;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileBar0, cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileBar1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar1;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileBar1, cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileBar2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileBar2;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileBar2, cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar2));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileGap1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap1;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileGap1, cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileGap2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap2;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileGap2, cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap2));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileGap3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileGap3;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileGap3, cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap3));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileExt0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt0;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileExt0, cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileExt1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt1;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileExt1, cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_TileExt2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_TileExt2;
    thisPFO->setAttribute( myAttribute_layerEnergy_TileExt2, cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt2));

    float layerEnergy_FCAL0 = cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL0);
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_FCAL0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL0;
    thisPFO->setAttribute( myAttribute_layerEnergy_FCAL0, layerEnergy_FCAL0);

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_FCAL1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL1;
    thisPFO->setAttribute( myAttribute_layerEnergy_FCAL1, cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_FCAL2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_FCAL2;
    thisPFO->setAttribute( myAttribute_layerEnergy_FCAL2, cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL2));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_MINIFCAL0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL0;
    thisPFO->setAttribute( myAttribute_layerEnergy_MINIFCAL0, cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL0));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_MINIFCAL1 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL1;
    thisPFO->setAttribute( myAttribute_layerEnergy_MINIFCAL1, cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL1));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_MINIFCAL2 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL2;
    thisPFO->setAttribute( myAttribute_layerEnergy_MINIFCAL2, cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL2));

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_MINIFCAL3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_MINIFCAL3;
    thisPFO->setAttribute( myAttribute_layerEnergy_MINIFCAL3, cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL3));

    //now set the layer energies for EMB3 and Tile0 - these are needed if we want to run a GSC style jet calibration, which is binned in EMB3 and Tile0 layer energies
    
    float layerEnergy_EM3 = layerEnergy_EMB3 + layerEnergy_EME3;

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EM3 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EM3;
    thisPFO->setAttribute( myAttribute_layerEnergy_EM3, layerEnergy_EM3);

    float layerEnergy_TileBar0 = cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0);
    float layerEnergy_TileExt0 = cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0);
    float layerEnergy_Tile0 = layerEnergy_TileBar0 + layerEnergy_TileExt0;

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_Tile0 = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_Tile0;
    thisPFO->setAttribute(myAttribute_layerEnergy_Tile0, layerEnergy_Tile0);

    //now set properties that are required for jet cleaning
    float layerEnergy_HEC = layerEnergy_HEC0 + layerEnergy_HEC1 + layerEnergy_HEC2 + layerEnergy_HEC3;

    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_HEC = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_HEC;
    thisPFO->setAttribute(myAttribute_layerEnergy_HEC, layerEnergy_HEC);

    float layerEnergy_EM = layerEnergy_preSamplerB + layerEnergy_preSamplerE + layerEnergy_EMB1 + layerEnergy_EMB2 + layerEnergy_EMB3 + layerEnergy_EME1 + layerEnergy_EME2 + layerEnergy_EME3 + layerEnergy_FCAL0;
    xAOD::PFODetails::PFOAttributes myAttribute_layerEnergy_EM = xAOD::PFODetails::PFOAttributes::eflowRec_LAYERENERGY_EM;
    thisPFO->setAttribute(myAttribute_layerEnergy_EM, layerEnergy_EM);
    
    float clusterTiming = cluster->time();

    xAOD::PFODetails::PFOAttributes myAttribute_TIMING = xAOD::PFODetails::PFOAttributes::eflowRec_TIMING;
    thisPFO->setAttribute(myAttribute_TIMING, clusterTiming);

  }
}

void PFONeutralCreatorAlgorithm::addMoment(const xAOD::CaloCluster::MomentType& momentType, const xAOD::PFODetails::PFOAttributes& pfoAttribute, const xAOD::CaloCluster& theCluster, xAOD::PFO& thePFO){

  double moment = 0.0;
  bool isRetrieved = theCluster.retrieveMoment(momentType, moment);
  if (true == isRetrieved) {
    xAOD::PFODetails::PFOAttributes myAttribute = pfoAttribute;
    float float_moment = static_cast<float>(moment);
    thePFO.setAttribute(myAttribute, float_moment);
  }
  else ATH_MSG_WARNING(" Could not retrieve moment from the CaloCluster");

}
