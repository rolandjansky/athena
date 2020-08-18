//eflowRec includes
#include "eflowRec/PFChargedFlowElementCreatorAlgorithm.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"

//EDM includes
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/FlowElementAuxContainer.h"
#include "xAODPFlow/PFODefs.h"
#include "xAODCore/AuxStoreAccessorMacros.h"

StatusCode PFChargedFlowElementCreatorAlgorithm::initialize(){

  ATH_CHECK(m_eflowCaloObjectContainerReadHandleKey.initialize());

  ATH_CHECK(m_chargedFlowElementContainerWriteHandleKey.initialize());
  
  return StatusCode::SUCCESS;
}

StatusCode PFChargedFlowElementCreatorAlgorithm::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG("Starting PFOChargedCreatorAlgorithm::execute");

  SG::WriteHandle<xAOD::FlowElementContainer> chargedFlowElementContainerWriteHandle(m_chargedFlowElementContainerWriteHandleKey,ctx);
  ATH_CHECK(chargedFlowElementContainerWriteHandle.record(std::make_unique<xAOD::FlowElementContainer>(),std::make_unique<xAOD::FlowElementAuxContainer>()));
  
  /* Create Charged FlowElements from all eflowCaloObjects */
  SG::ReadHandle<eflowCaloObjectContainer> eflowCaloObjectContainerReadHandle(m_eflowCaloObjectContainerReadHandleKey,ctx);
  for (const auto& thisEflowCaloObject : *eflowCaloObjectContainerReadHandle) createChargedFlowElements(*thisEflowCaloObject,true,chargedFlowElementContainerWriteHandle);

  std::sort(chargedFlowElementContainerWriteHandle->begin(), chargedFlowElementContainerWriteHandle->end(), [] (const xAOD::FlowElement* flowElement1, const xAOD::FlowElement* flowElement2) {return flowElement1->pt()>flowElement2->pt();});

  return StatusCode::SUCCESS;  

}

void PFChargedFlowElementCreatorAlgorithm::createChargedFlowElements(const eflowCaloObject& energyFlowCaloObject, bool addClusters, SG::WriteHandle<xAOD::FlowElementContainer>& chargedFlowElementContainerWriteHandle) const {

  /* Loop over all tracks in the eflowCaloObject */
  for (unsigned int iTrack = 0; iTrack < energyFlowCaloObject.nTracks(); ++iTrack) {

    eflowRecTrack* efRecTrack = energyFlowCaloObject.efRecTrack(iTrack);

    /* Skip tracks that haven't been subtracted */
    if (!m_eOverPMode && !efRecTrack->isSubtracted()){continue;}

    /* Create new xAOD::FlowElement and set the type to charged PFlow */
    xAOD::FlowElement* thisFE = new xAOD::FlowElement();
    chargedFlowElementContainerWriteHandle->push_back(thisFE);
    thisFE->setSignalType(xAOD::FlowElement::SignalType::ChargedPFlow);

    /* Get the track elementLink and add it to the xAOD:FE. 
    Note we first have to convert it to an IParticle ElementLink. */
    ElementLink<xAOD::TrackParticleContainer> theTrackLink = efRecTrack->getTrackElemLink();
    ElementLink< xAOD::IParticleContainer > theIParticleTrackLink; 
    theIParticleTrackLink.resetWithKeyAndIndex(theTrackLink.persKey(),theTrackLink.persIndex() ); 
    std::vector<ElementLink<xAOD::IParticleContainer> > vecIParticleTrackLinkContainer;
    vecIParticleTrackLinkContainer.push_back(theIParticleTrackLink);
    thisFE->setChargedObjectLinks(vecIParticleTrackLinkContainer);

    //Now set the charge
    thisFE->setCharge(efRecTrack->getTrack()->charge());

    std::pair<double,double> etaPhi(0.0,0.0);

    if (m_eOverPMode){
      /* In EOverPMode want charged eflowObjects to have extrapolated eta,phi as coordinates
       * (needed for analysis of EOverP Data) */
      etaPhi = efRecTrack->getTrackCaloPoints().getEM2etaPhi();

      /*add information to xAOD*/
      const static SG::AuxElement::Accessor<int> accLHED("LayerHED");
      accLHED(*thisFE) = efRecTrack->getLayerHED();

      const static SG::AuxElement::Accessor<std::vector<int> > accCellOrderVector("LayerVectorCellOrdering");
      accCellOrderVector(*thisFE) = efRecTrack->getLayerCellOrderVector();

      const static SG::AuxElement::Accessor<std::vector<float> > accRadiusCellOrderVector("RadiusVectorCellOrdering");
      accRadiusCellOrderVector(*thisFE) = efRecTrack->getRadiusCellOrderVector();

      const static SG::AuxElement::Accessor<std::vector<float> > accAvgEDensityCellOrderVector("AvgEdensityVectorCellOrdering");
      accAvgEDensityCellOrderVector(*thisFE) = efRecTrack->getAvgEDensityCellOrderVector();
    } else {
      /* In normal mode we want the track eta,phi at the perigee */
      etaPhi.first = efRecTrack->getTrack()->eta();
      etaPhi.second = efRecTrack->getTrack()->phi();
    }

    /* Set the 4-vector of the xAOD::PFO */
    thisFE->setP4(efRecTrack->getTrack()->pt(), etaPhi.first, etaPhi.second, efRecTrack->getTrack()->m());

    ATH_MSG_DEBUG("Created charged PFO with E, pt, eta and phi of " << thisFE->e() << ", " << thisFE->pt() << ", " << thisFE->eta() << " and " << thisFE->phi());

    /* Add the amount of energy the track was expected to deposit in the calorimeter - this is needed to calculate the charged weight in the jet finding */
    const static SG::AuxElement::Accessor<float> accTracksExpectedEnergyDeposit("TracksExpectedEnergyDeposit");
    accTracksExpectedEnergyDeposit(*thisFE) = efRecTrack->getEExpect();
    ATH_MSG_DEBUG("Have set that PFO's expected energy deposit to be " << efRecTrack->getEExpect());

    /* Flag if this track was in a dense environment for later checking */
    //There is an issue using bools - when written to disk they convert to chars. So lets store the bool as an int.
    const static SG::AuxElement::Accessor<int> accIsInDenseEnvironment("IsInDenseEnvironment");
    accIsInDenseEnvironment(*thisFE) = efRecTrack->isInDenseEnvironment();

     /* Optionally we add the links to clusters to the xAOD::PFO */
    if (addClusters){

      std::vector<std::pair<eflowTrackClusterLink*,float> > trackClusterLinkPairs = energyFlowCaloObject.efRecLink();

      std::vector<eflowTrackClusterLink*> thisTracks_trackClusterLinks = efRecTrack->getClusterMatches();

      /** Each eflowCaloObject has a list of clusters for all the tracks it represents.
       *  We only want the subset of the clusters matched to this track, and collect these in thisTracks_trackClusterLinksSubtracted.
       */

      std::vector<eflowTrackClusterLink*> thisTracks_trackClusterLinksSubtracted;

      //Create vector of pairs which map each CaloCluster to the ratio of its new energy to unstracted energy
      std::vector<std::pair<ElementLink<xAOD::CaloClusterContainer>, double> > vectorClusterToSubtractedEnergies;

      for (auto& trackClusterLink : thisTracks_trackClusterLinks){
        for (auto& trackClusterLinkPair : trackClusterLinkPairs){
          if (!m_eOverPMode && trackClusterLinkPair.first == trackClusterLink && !std::isnan(trackClusterLinkPair.second)) {
            thisTracks_trackClusterLinksSubtracted.push_back(trackClusterLink);
            eflowRecCluster* efRecCluster = trackClusterLinkPair.first->getCluster();
            ElementLink<xAOD::CaloClusterContainer> theOriginalClusterLink = efRecCluster->getOriginalClusElementLink();
              ElementLink<xAOD::CaloClusterContainer> theSisterClusterLink = (*theOriginalClusterLink)->getSisterClusterLink();
            if (theSisterClusterLink.isValid()) vectorClusterToSubtractedEnergies.push_back(std::pair(theSisterClusterLink,trackClusterLinkPair.second));
            else vectorClusterToSubtractedEnergies.push_back(std::pair(theOriginalClusterLink,trackClusterLinkPair.second));
          }
          else if (m_eOverPMode) thisTracks_trackClusterLinksSubtracted.push_back(trackClusterLink);
        }
      }

      //sort the vectorClusterToSubtractedEnergies in order of subtracted energy ratio from low (most subtracted) to high (least subtracted)
      std::sort(vectorClusterToSubtractedEnergies.begin(),vectorClusterToSubtractedEnergies.end(), [](auto const& a, auto const&b){return a.second < b.second;});
      //now split this into two vectors, ready to be used by the FlowElement
      std::vector<ElementLink<xAOD::IParticleContainer> > theClusters;
      std::vector<float> theClusterWeights;
      for (auto thePair : vectorClusterToSubtractedEnergies){
        ElementLink< xAOD::IParticleContainer > theIParticleTrackLink; 
        theIParticleTrackLink.resetWithKeyAndIndex(thePair.first.persKey(),thePair.first.persIndex()); 
        theClusters.push_back(theIParticleTrackLink);
        theClusterWeights.push_back(thePair.second);
      }

      //Set the vector of clusters and vector of corresponding energies.
      thisFE->setOtherObjectLinks(theClusters,theClusterWeights);

    }//if we add clusters

  }//loop over eflowRecTracks

}
