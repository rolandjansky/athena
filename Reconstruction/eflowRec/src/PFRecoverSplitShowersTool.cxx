/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/PFRecoverSplitShowersTool.h"
#include "eflowRec/eflowUtil.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowCaloObjectMaker.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowCellList.h"
#include "eflowRec/IEFlowCellEOverPTool.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowSubtractor.h"
#include "eflowRec/PFSubtractionStatusSetter.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

using namespace eflowSubtract;

PFRecoverSplitShowersTool::PFRecoverSplitShowersTool(const std::string& type,const std::string& name,const IInterface* parent):
  base_class(type, name, parent),
  m_binnedParameters(std::make_unique<eflowEEtaBinnedParameters>())
{
}

PFRecoverSplitShowersTool::~PFRecoverSplitShowersTool() {}

StatusCode PFRecoverSplitShowersTool::initialize(){

  if (m_theEOverPTool.retrieve().isFailure()){
    ATH_MSG_WARNING("Cannot find eflowEOverPTool");
    return StatusCode::SUCCESS;
  }

  if (m_theEOverPTool->fillBinnedParameters(m_binnedParameters.get()).isFailure()){
    ATH_MSG_WARNING("Could not execute eflowCellEOverPTool");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

void PFRecoverSplitShowersTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, eflowRecTrackContainer* eflowRecTracks, eflowRecClusterContainer* eflowRecClusters) const {

  ATH_MSG_DEBUG("Executing");

  eflowData data;
  data.caloObjects = theEflowCaloObjectContainer;
  data.tracksToRecover.reserve(eflowRecTracks->size());
  data.clustersToConsider.reserve(eflowRecClusters->size());

  fillTracksToRecover(data);
  fillClustersToConsider(data);

  /* Add each track to its matching cluster's eflowCaloObject */
  int const nOriginalObj = matchAndCreateEflowCaloObj(data);

  /* Extrapolate tracks, match clusters, do shower simulation, run cell subtraction */
  performRecovery(nOriginalObj,data);

}

StatusCode PFRecoverSplitShowersTool::finalize(){
  return StatusCode::SUCCESS;

}

void PFRecoverSplitShowersTool::fillClustersToConsider(eflowData& data) const {

  data.clustersToConsider.clear();

  for (auto thisEflowCaloObject : *data.caloObjects){

    if (thisEflowCaloObject->nClusters() == 0) { continue; }

    for(unsigned i=0; i<thisEflowCaloObject->nClusters(); ++i){
        /* Skip empty clusters (subtraction remnants) */
        const CaloClusterCellLink* theCellLink = thisEflowCaloObject->efRecCluster(i)->getCluster()->getCellLinks();
        if (0 == (int)theCellLink->size()){ continue; }

        thisEflowCaloObject->efRecCluster(i)->clearTrackMatches();
        data.clustersToConsider.insert(thisEflowCaloObject->efRecCluster(i));
        thisEflowCaloObject->clearClusters();
    }
  }
}

void PFRecoverSplitShowersTool::fillTracksToRecover(eflowData& data) const {

  data.tracksToRecover.clear(); 
  for (auto thisEflowCaloObject : *data.caloObjects){

    /* Skip isolated tracks if flag set */
    if (!m_recoverIsolatedTracks && thisEflowCaloObject->nClusters() == 0) {
      unsigned int nTrk = thisEflowCaloObject->nTracks();
      // But make sure we get eflowObjects from them
      for (unsigned int iTrk = 0; iTrk < nTrk; ++iTrk) {
	      eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrk);
      	if (!thisEfRecTrack->isSubtracted()) thisEfRecTrack->setSubtracted();
      }
      continue;
    }

    /* Add all tracks on the eflowCaloObject that haven't been subtracted yet*/
    std::vector<eflowRecTrack*> updatedTracks; updatedTracks.clear();
    unsigned int nTracks = thisEflowCaloObject->nTracks();

    /* For cluster-only CaloObjects */
    if(nTracks == 0) continue;

    /* For track-only and track-cluster CaloObjects */
    for (unsigned int iTrack = 0; iTrack < nTracks; ++iTrack){
      eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrack);
      if (thisEfRecTrack->isSubtracted()){
        updatedTracks.push_back(thisEfRecTrack);
        continue;
      }
      thisEfRecTrack->clearClusterMatches();
      data.tracksToRecover.push_back(thisEfRecTrack);
    }
    thisEflowCaloObject->clearTracks();
    if (!updatedTracks.empty()) {
      thisEflowCaloObject->addTracks(updatedTracks);
      updatedTracks.clear();
    } else {
      thisEflowCaloObject->clearLinks();
    }
  }

  std::sort(data.tracksToRecover.begin(),data.tracksToRecover.end(),eflowRecTrack::SortDescendingPt());

}

unsigned int PFRecoverSplitShowersTool::matchAndCreateEflowCaloObj(eflowData& data) const {
  /* Cache the original number of eflowCaloObjects */
  const unsigned int nCaloObj = data.caloObjects->size();

  /* loop tracks in data.tracksToRecover and do matching */
  for (auto thisEfRecTrack : data.tracksToRecover){
 
    /* No point to do anything if bin does not exist */
    if (!thisEfRecTrack->hasBin()) {
      std::unique_ptr<eflowCaloObject> thisEflowCaloObject = std::make_unique<eflowCaloObject>();
      thisEflowCaloObject->addTrack(thisEfRecTrack);
      data.caloObjects->push_back(std::move(thisEflowCaloObject));
      continue;
    }
    if (msgLvl(MSG::DEBUG)){
      const xAOD::TrackParticle* track = thisEfRecTrack->getTrack();
      ATH_MSG_DEBUG("Recovering charged EFO with e,pt, eta and phi " << track->e() << ", " << track->pt() << ", " << track->eta() << " and " << track->phi());
    }
    // Get list of matched clusters in the dR<0.2 cone -- already identified
    const std::vector<eflowTrackClusterLink*>* matchedClusters_02 = thisEfRecTrack->getAlternativeClusterMatches("cone_02");
    if (!matchedClusters_02) { continue; }

    if (msgLvl(MSG::DEBUG)){
      for (auto trkClusLink : *matchedClusters_02) {
	const eflowRecCluster* thisEFRecCluster = trkClusLink->getCluster();
	ATH_MSG_DEBUG("Have matched cluster with e, pt, eta, phi of " << thisEFRecCluster->getCluster()->e() << ", " <<  thisEFRecCluster->getCluster()->eta() << ", " << thisEFRecCluster->getCluster()->eta() << " and " << thisEFRecCluster->getCluster()->phi());
      }
    }

    if (matchedClusters_02->empty()) { continue; }

    /* Matched cluster: create TrackClusterLink and add it to both the track and the cluster (eflowCaloObject will be created later) */
    for (auto trkClusLink : *matchedClusters_02){
      eflowRecCluster* thisEFRecCluster = trkClusLink->getCluster();
      // Look up whether this cluster is intended for recovery
      if( data.clustersToConsider.find(trkClusLink->getCluster()) == data.clustersToConsider.end() ) {continue;}
      eflowTrackClusterLink* trackClusterLink = eflowTrackClusterLink::getInstance(thisEfRecTrack,thisEFRecCluster);
      thisEfRecTrack->addClusterMatch(trackClusterLink);
      thisEFRecCluster->addTrackMatch(trackClusterLink);
    }
  }

  /* Create all eflowCaloObjects that contain eflowRecCluster */
  eflowCaloObjectMaker makeCaloObject;
  std::vector<eflowRecCluster*> v_clustersToConsider(data.clustersToConsider.begin(),data.clustersToConsider.end());
  std::sort(v_clustersToConsider.begin(),v_clustersToConsider.end(),eflowRecCluster::SortDescendingPt());
  unsigned int nCaloObjects = makeCaloObject.makeTrkCluCaloObjects(data.tracksToRecover, v_clustersToConsider,
								   data.caloObjects);
  ATH_MSG_DEBUG("PFRecoverSplitShowersTool created " << nCaloObjects << " CaloObjects");

  const double gaussianRadius = 0.032;
  const double gaussianRadiusError = 1.0e-3;
  const double maximumRadiusSigma = 3.0;
  
  // Should move to a common header or similar, as shared with PFCellLevelSubtractionTool
  eflowLayerIntegrator integrator(gaussianRadius, gaussianRadiusError, maximumRadiusSigma, m_isHLLHC);

  /* integrate cells; determine FLI; eoverp */
  for (unsigned int iCalo = nCaloObj; iCalo < data.caloObjects->size(); ++iCalo) {
    eflowCaloObject* thisEflowCaloObject = data.caloObjects->at(iCalo);
    thisEflowCaloObject->simulateShower(&integrator, m_binnedParameters.get(), m_useUpdated2015ChargedShowerSubtraction);
  }
  return nCaloObj;
}

void PFRecoverSplitShowersTool::performSubtraction(eflowCaloObject* thisEflowCaloObject) const {

  PFSubtractionStatusSetter pfSubtractionStatusSetter;

  for (unsigned iTrack = 0; iTrack < thisEflowCaloObject->nTracks(); ++iTrack) {
    eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrack);
    ATH_MSG_DEBUG("About to recover track with e, pt, eta and phi of " << thisEfRecTrack->getTrack()->e() << ", " << thisEfRecTrack->getTrack()->pt() << ", " << thisEfRecTrack->getTrack()->eta() << " and "
    << thisEfRecTrack->getTrack()->eta());
    /* Get matched cluster via Links */
    std::vector<eflowRecCluster*> matchedClusters;
    matchedClusters.clear();
    std::vector<eflowTrackClusterLink*> links = thisEfRecTrack->getClusterMatches();
    for ( auto thisEFlowTrackClusterLink : links) matchedClusters.push_back(thisEFlowTrackClusterLink->getCluster());
    std::sort(matchedClusters.begin(),matchedClusters.end(),eflowRecCluster::SortDescendingPt());

    if (msgLvl(MSG::DEBUG)){
      for (auto thisClus : matchedClusters) ATH_MSG_DEBUG("Cluster " << thisClus->getCluster()->index() << " with e,pt, eta and phi of " << thisClus->getCluster()->e() << ", "<< thisClus->getCluster()->pt() << ", " << thisClus->getCluster()->eta() << " and " << thisClus->getCluster()->phi() << " will be subtracted");
    }
    /* Do subtraction */
    std::vector<std::pair<xAOD::CaloCluster*, bool> > clusterSubtractionList;
    clusterSubtractionList.reserve(matchedClusters.size());
    std::map<xAOD::CaloCluster*, double> clusterEnergyMap;
    for (auto thisEFlowRecCluster : matchedClusters) {
      xAOD::CaloCluster* thisCluster = thisEFlowRecCluster->getCluster();
      clusterSubtractionList.push_back(std::pair(thisCluster,false));
      clusterEnergyMap[thisCluster] = thisCluster->e();
    }

    if (getSumEnergy(clusterSubtractionList) - thisEfRecTrack->getEExpect() < m_subtractionSigmaCut
        * sqrt(thisEfRecTrack->getVarEExpect())) {
      /* Check if we can annihilate right away */
      Subtractor::annihilateClusters(clusterSubtractionList);
      //Now we should mark all of these clusters as being subtracted
      //Now need to mark which clusters were modified in the subtraction procedure
      pfSubtractionStatusSetter.markAnnihStatus(*thisEflowCaloObject);
    } else {
      /* Subtract the track from all matched clusters */
      const bool debugToggle = msgLvl(MSG::DEBUG) ? true : false;
      Subtractor::subtractTracksFromClusters(thisEfRecTrack, clusterSubtractionList, debugToggle);                

      /* Annihilate the cluster(s) if the remnant is small (i.e. below k*sigma) */
      if (getSumEnergy(clusterSubtractionList) < m_subtractionSigmaCut
          * sqrt(thisEfRecTrack->getVarEExpect())) {
        Subtractor::annihilateClusters(clusterSubtractionList);
	      //Now we should mark all of these clusters as being subtracted
	      pfSubtractionStatusSetter.markAnnihStatus(*thisEflowCaloObject);
      }
      else{
        //Else now to mark which clusters were modified in the subtraction procedure
        std::vector<float> clusterSubtractedEnergyRatios;
        for (auto thisCluster: clusterSubtractionList) {
          if (fabs(thisCluster.first->e() - clusterEnergyMap[thisCluster.first]) > 0.0001) clusterSubtractedEnergyRatios.push_back(thisCluster.first->e()/clusterEnergyMap[thisCluster.first]);
          else clusterSubtractedEnergyRatios.push_back(NAN);
        }

	pfSubtractionStatusSetter.markSubtractionStatus(clusterSubtractionList, clusterSubtractedEnergyRatios, *thisEflowCaloObject);
	
        } 

    }

    if (msgLvl(MSG::DEBUG)){
      for (auto thisClus : matchedClusters) ATH_MSG_DEBUG("Cluster with e,pt, eta and phi of " << thisClus->getCluster()->e() << ", "<< thisClus->getCluster()->pt() << ", " << thisClus->getCluster()->eta() << " and " << thisClus->getCluster()->phi() << " has been subtracted");
    } 

    /* Flag tracks as subtracted */
    thisEfRecTrack->setSubtracted();
  }
}

void PFRecoverSplitShowersTool::performRecovery(unsigned int const nOriginalObj, eflowData& data) const {
  unsigned int nEFCaloObs = data.caloObjects->size();
  for (unsigned int iCalo = nOriginalObj; iCalo < nEFCaloObs; ++iCalo) {
    eflowCaloObject* thisEflowCaloObject = data.caloObjects->at(iCalo);
    performSubtraction(thisEflowCaloObject);
  }

}

double PFRecoverSplitShowersTool::getSumEnergy(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusters) const {
  double result = 0.0;
  for (auto thisPair : clusters) result += (thisPair.first)->e();
  return result;
}
