#include "eflowRec/PFOChargedCreatorAlgorithm.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"

#include "xAODPFlow/PFOAuxContainer.h"

PFOChargedCreatorAlgorithm::PFOChargedCreatorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
}

StatusCode PFOChargedCreatorAlgorithm::initialize(){

  ATH_CHECK(m_trackVertexAssociationTool.retrieve());

  ATH_CHECK(m_vertexContainerReadHandleKey.initialize());
  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());

  ATH_CHECK(m_chargedPFOContainerWriteHandleKey.initialize());
  
  return StatusCode::SUCCESS;
}

void PFOChargedCreatorAlgorithm::execute(const eflowCaloObject& energyFlowCaloObject, SG::WriteHandle<xAOD::PFOContainer>& chargedPFOContainerWriteHandle){

  ATH_MSG_DEBUG("Processing eflowCaloObject");
  
  if (m_eOverPMode) createChargedPFO(energyFlowCaloObject, true, chargedPFOContainerWriteHandle);
  else createChargedPFO(energyFlowCaloObject,false,chargedPFOContainerWriteHandle);

  SG::ReadHandle<xAOD::VertexContainer> vertexContainerReadHandle(m_vertexContainerReadHandleKey);
  const xAOD::VertexContainer* theVertexContainer = vertexContainerReadHandle.ptr();  
  addVertexLinksToChargedPFO(*theVertexContainer, chargedPFOContainerWriteHandle);
  
}

StatusCode  PFOChargedCreatorAlgorithm::execute(){

  ATH_MSG_DEBUG("Processing eflowCaloObjectContainer");

  SG::WriteHandle<xAOD::PFOContainer> chargedPFOContainerWriteHandle(m_chargedPFOContainerWriteHandleKey);
  ATH_CHECK(chargedPFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(),std::make_unique<xAOD::PFOAuxContainer>()));
  
  /* Create Charged PFOs from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(m_eflowCaloObjectContainerReadHandleKey);
  for (auto thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) execute(*thisEflowCaloObject,chargedPFOContainerWriteHandle);

  return StatusCode::SUCCESS;  
}

StatusCode PFOChargedCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS; }

void PFOChargedCreatorAlgorithm::createChargedPFO(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::PFOContainer>& chargedPFOContainerWriteHandle){

  /* Loop over all tracks in the eflowCaloObject */
  int nTracks = energyFlowCaloObject.nTracks();
  for (int iTrack = 0; iTrack < nTracks; ++iTrack) {

    eflowRecTrack* efRecTrack = energyFlowCaloObject.efRecTrack(iTrack);

    /* Skip tracks that haven't been subtracted */
    if (false == m_eOverPMode){
      if (!efRecTrack->isSubtracted()){ continue; }
    }
    
    /* Create new xAOD::PFO */
    xAOD::PFO* thisPFO = new xAOD::PFO();
    chargedPFOContainerWriteHandle->push_back(thisPFO);

    /* Get the track elementLink and add it to the xAOD:PFO  */
    ElementLink<xAOD::TrackParticleContainer> theTrackLink = efRecTrack->getTrackElemLink();
    bool isSet = thisPFO->setTrackLink(theTrackLink);
    if (!isSet) ATH_MSG_WARNING("Could not set Track B in PFO ");
    thisPFO->setCharge(efRecTrack->getTrack()->charge());

    std::pair<double,double> etaPhi(0.0,0.0);

    if (m_eOverPMode){
      /* In EOverPMode want charged eflowObjects to have extrapolated eta,phi as coordinates
       * (needed for analysis of EOverP Data) */
      etaPhi = efRecTrack->getTrackCaloPoints().getEM2etaPhi();
        
      /*add information to xAOD*/
      xAOD::PFODetails::PFOAttributes myAttribute_layerHED = xAOD::PFODetails::PFOAttributes::eflowRec_layerHED;
      thisPFO->setAttribute<int>(myAttribute_layerHED,efRecTrack->getLayerHED() );
      
      xAOD::PFODetails::PFOAttributes myAttribute_layerVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_layerVectorCellOrdering;
      thisPFO->setAttribute<std::vector<int> >(myAttribute_layerVectorCellOrdering,efRecTrack->getLayerCellOrderVector() );
      
      xAOD::PFODetails::PFOAttributes myAttribute_radiusVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_radiusVectorCellOrdering;
      thisPFO->setAttribute<std::vector<float> >(myAttribute_radiusVectorCellOrdering,efRecTrack->getRadiusCellOrderVector() );
        
      xAOD::PFODetails::PFOAttributes myAttribute_avgEdensityVectorCellOrdering = xAOD::PFODetails::PFOAttributes::eflowRec_avgEdensityVectorCellOrdering;
      thisPFO->setAttribute<std::vector<float> >(myAttribute_avgEdensityVectorCellOrdering,efRecTrack->getAvgEDensityCellOrderVector() );              
    } else {
      /* In normal mode we want the track eta,phi at the perigee */
      etaPhi.first = efRecTrack->getTrack()->eta();
      etaPhi.second = efRecTrack->getTrack()->phi();
    }
    /* Set the 4-vector of the xAOD::PFO */
    thisPFO->setP4(efRecTrack->getTrack()->pt(), etaPhi.first, etaPhi.second, efRecTrack->getTrack()->m());

    ATH_MSG_DEBUG("Created charged PFO with E, pt, eta and phi of " << thisPFO->e() << ", " << thisPFO->pt() << ", " << thisPFO->eta() << " and " << thisPFO->phi());
    
    /* Add the amount of energy the track was expected to deposit in the calorimeter - this is needed to calculate the charged weight in the jet finding */
    xAOD::PFODetails::PFOAttributes myAttribute_tracksExpectedEnergyDeposit = xAOD::PFODetails::PFOAttributes::eflowRec_tracksExpectedEnergyDeposit;
    thisPFO->setAttribute<float>(myAttribute_tracksExpectedEnergyDeposit,efRecTrack->getEExpect() );
    
    /* Flag if this track was in a dense environment for later checking */
    xAOD::PFODetails::PFOAttributes myAttribute_isInDenseEnvironment = xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment;
    //There is some issue using bools - when written to disk they convert to chars. So lets store the bool as an int.
    thisPFO->setAttribute<int>(myAttribute_isInDenseEnvironment, (efRecTrack->isInDenseEnvironment()));

    /* Optionally we add the links to clusters to the xAOD::PFO */
    if (true == addClusters){
       unsigned int nClusters = energyFlowCaloObject.nClusters();
       for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){
	 eflowRecCluster* thisEfRecCluster = energyFlowCaloObject.efRecCluster(iCluster);
	 ElementLink<xAOD::CaloClusterContainer> theClusLink = thisEfRecCluster->getClusElementLink();
	 bool isSet = thisPFO->addClusterLink(theClusLink);
	 if (!isSet) ATH_MSG_WARNING("Could not set Cluster in PFO");
       }//cluster loop
    }//addClusters is set to true - so we added the clusters to the xAOD::PFO   

  }//loop over the tracks on the eflowCaloObject
}

void PFOChargedCreatorAlgorithm::addVertexLinksToChargedPFO(const xAOD::VertexContainer& theVertexContainer, SG::WriteHandle<xAOD::PFOContainer>& chargedPFOContainerWriteHandle){

  //This is a loop on all xAOD::PFO with non-zero charge
  for (auto theChargedPFO : *(chargedPFOContainerWriteHandle.ptr())){
    const xAOD::TrackParticle* theTrack = theChargedPFO->track(0);
    if (theTrack){
      ElementLink< xAOD::VertexContainer> theVertexLink = m_trackVertexAssociationTool->getUniqueMatchVertexLink(*theTrack,theVertexContainer);
      bool haveSetLink = theChargedPFO->setVertexLink(theVertexLink);
      if (!haveSetLink) ATH_MSG_WARNING(" Could not set vertex link on charged PFO");
    }//if valid pointer to xAOD::TrackParticle
  }

}
