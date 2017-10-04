#include "eflowRec/PFOChargedCreatorAlgorithm.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"

#include "xAODPFlow/PFOAuxContainer.h"

PFOChargedCreatorAlgorithm::PFOChargedCreatorAlgorithm(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator) ,
    m_eOverPMode(false),
    m_trackVertexAssociationTool("",this),
    m_vertexContainerReadHandle("PrimaryVertices"),
    m_eflowCaloObjectContainerReadHandle("eflowCaloObjects"),
    m_chargedPFOContainerWriteHandle("JetETMissChargedParticleFlowObjects")
{
  /* Name of  eflow Container to be created */
  declareProperty("EOverPMode", m_eOverPMode);
  declareProperty("TrackVertexAssociationTool", m_trackVertexAssociationTool);
  declareProperty("VertexContainerName",m_vertexContainerReadHandle);
  declareProperty("PFOOutputName", m_chargedPFOContainerWriteHandle);
}

StatusCode PFOChargedCreatorAlgorithm::initialize(){

  ATH_CHECK(m_trackVertexAssociationTool.retrieve());

  ATH_CHECK(m_vertexContainerReadHandle.initialize());
  ATH_CHECK(m_eflowCaloObjectContainerReadHandle.initialize());

  ATH_CHECK(m_chargedPFOContainerWriteHandle.initialize());
  
  return StatusCode::SUCCESS;
}

void PFOChargedCreatorAlgorithm::execute(const eflowCaloObject& energyFlowCaloObject){

  ATH_MSG_DEBUG("Processing eflowCaloObject");
  
  if (m_eOverPMode) createChargedPFO(energyFlowCaloObject, true);
  else createChargedPFO(energyFlowCaloObject);

  const xAOD::VertexContainer* theVertexContainer = m_vertexContainerReadHandle.ptr();  
  addVertexLinksToChargedPFO(*theVertexContainer);
  
}

StatusCode  PFOChargedCreatorAlgorithm::execute(){

  ATH_MSG_DEBUG("Processing eflowCaloObjectContainer");

  ATH_CHECK(m_chargedPFOContainerWriteHandle.record(std::make_unique<xAOD::PFOContainer>(),std::make_unique<xAOD::PFOAuxContainer>()));
  
  /* Create Charged PFOs from all eflowCaloObjects */
  unsigned int nEFCaloObs = m_eflowCaloObjectContainerReadHandle->size();
  for (unsigned int iEFCalOb = 0; iEFCalOb < nEFCaloObs; ++iEFCalOb){
    const eflowCaloObject* thisEflowCaloObject = m_eflowCaloObjectContainerReadHandle->at(iEFCalOb);
    execute(*thisEflowCaloObject);
  }

  return StatusCode::SUCCESS;
  
}

StatusCode PFOChargedCreatorAlgorithm::finalize(){ return StatusCode::SUCCESS; }

void PFOChargedCreatorAlgorithm::createChargedPFO(const eflowCaloObject& energyFlowCaloObject, bool addClusters){

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
    m_chargedPFOContainerWriteHandle->push_back(thisPFO);

    /* Get the track elementLink and add it to the xAOD:PFO  */
    ElementLink<xAOD::TrackParticleContainer> theTrackLink = efRecTrack->getTrackElemLink();
    bool isSet = thisPFO->setTrackLink(theTrackLink);
    if (!isSet) { msg(MSG::WARNING) << "Could not set Track B in PFO " << endmsg; }
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
      /* In normal mode we want te track eta,phi at the perigee */
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
	 if (!isSet) msg(MSG::WARNING) << "Could not set Cluster in PFO " << endmsg;
       }//cluster loop
    }//addClusters is set to true - so we added the clusters to the xAOD::PFO   

  }//loop over the tracks on the eflowCaloObject
}

void PFOChargedCreatorAlgorithm::addVertexLinksToChargedPFO(const xAOD::VertexContainer& theVertexContainer){

  //This is a loop on all xAOD::PFO with non-zero charge
  for (auto theChargedPFO : *m_chargedPFOContainerWriteHandle){
    const xAOD::TrackParticle* theTrack = theChargedPFO->track(0);
    if (theTrack){
      ElementLink< xAOD::VertexContainer> theVertexLink = m_trackVertexAssociationTool->getUniqueMatchVertexLink(*theTrack,theVertexContainer);
      bool haveSetLink = theChargedPFO->setVertexLink(theVertexLink);
      if (!haveSetLink) msg(MSG::WARNING) << " Could not set vertex link on charged PFO " << endmsg;
    }//if valid pointer to xAOD::TrackParticle
  }

}
