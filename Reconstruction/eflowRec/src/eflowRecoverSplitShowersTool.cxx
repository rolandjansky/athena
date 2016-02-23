/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME: eflowRecoverSplitShowersTool.cxx
PACKAGE: offline/Reconstruction/eflowRec

AUTHOR: Thomas Velz, Mark Hodgkinson
CREATED: 16 January 2014

********************************************************************/

#include "eflowRec/eflowRecoverSplitShowersTool.h"
#include "eflowRec/eflowUtil.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowCellList.h"
#include "eflowRec/eflowCellEOverPTool.h"
#include "eflowRec/PFTrackClusterMatchingTool.h"
#include "eflowRec/eflowBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowCellSubtractionManager.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

eflowRecoverSplitShowersTool::eflowRecoverSplitShowersTool(const std::string& type,const std::string& name,const IInterface* parent):
AthAlgTool(type, name, parent),
m_debug(0),
m_rCell(0.75),
m_windowRms(0.032),
m_theEOverPTool("eflowCellEOverPTool",this),
m_matchingTool("PFTrackClusterMatchingTool/RcvrSpltMatchingTool", this),
m_binnedParameters(new eflowBinnedParameters()),
m_integrator(new eflowLayerIntegrator(m_windowRms, 1.0e-3, 3.0)),
m_subtractionSigmaCut(1.5),
m_recoverIsolatedTracks(false),
m_nTrackClusterMatches(0)
{
  declareInterface<eflowRecoverSplitShowersTool>(this);
  declareProperty("SubtractionSigmaCut",m_subtractionSigmaCut);
  declareProperty("eflowCellEOverPTool", m_theEOverPTool,"Energy Flow E/P Values and Shower Parameters Tool");
  declareProperty("PFTrackClusterMatchingTool", m_matchingTool, "The track-cluster matching tool");
  declareProperty("RecoverIsolatedTracks",m_recoverIsolatedTracks,"Whether to recover isolated tracks also");
  eflowCellSubtractionManager::setRMaxAndWeightRange(m_rCell, 1.0e6);
}

eflowRecoverSplitShowersTool::~eflowRecoverSplitShowersTool() {}

StatusCode eflowRecoverSplitShowersTool::initialize(){

  // tool service
  IToolSvc* myToolSvc;
  if ( service("ToolSvc",myToolSvc).isFailure() ) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endreq;
    return StatusCode::SUCCESS;
  }

  if (m_matchingTool.retrieve().isFailure()){
    msg(MSG::WARNING) << "Couldn't retrieve PFTrackClusterMatchingTool." << endreq;
    return StatusCode::SUCCESS;
  }

  if (m_theEOverPTool.retrieve().isFailure()){
    msg(MSG::WARNING) << "Cannot find eflowEOverPTool" << endreq;
    return StatusCode::SUCCESS;
  }

  if (m_theEOverPTool->execute(m_binnedParameters).isFailure()){
    msg(MSG::WARNING) << "Could not execute eflowCellEOverPTool " << endreq;
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

void eflowRecoverSplitShowersTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer){

  msg(MSG::DEBUG) << "Executing eflowRecoverSplitShowersTool" << endreq;

  m_eflowCaloObjectContainer = theEflowCaloObjectContainer;

  /* Get all neutral clusters (including charged e/p failures) */
  getClustersToConsider();

  /* Pick tracks to "recover", try to match neutrals to them, subtract, annihilate */
  getTracksToRecover();

  /* Extrapolate tracks, match clusters, do shower simulation, run cell subtraction */
  performRecovery();
}

StatusCode eflowRecoverSplitShowersTool::finalize(){

  msg(MSG::INFO) << "Produced " << m_nTrackClusterMatches << " track-cluster matches." << endreq;

  delete m_binnedParameters;
  delete m_integrator;

  return StatusCode::SUCCESS;

}

void eflowRecoverSplitShowersTool::getClustersToConsider() {

  m_clustersToConsider.clear();

  eflowCaloObjectContainer::iterator  itEFCalObject = m_eflowCaloObjectContainer->begin();
  eflowCaloObjectContainer::iterator endEFCalObject = m_eflowCaloObjectContainer->end();
  for (; itEFCalObject != endEFCalObject; ++itEFCalObject) {
    eflowCaloObject* thisEflowCaloObject = *itEFCalObject;

    if (thisEflowCaloObject->nClusters() == 0) { continue; }

    if (thisEflowCaloObject->nClusters() != 1) {
      msg(MSG::WARNING) << "eflowCaloObject has "<< thisEflowCaloObject->nClusters() << " instead of 1 cluster! Will not use them for split shower recovery." << endreq;
      continue;
    }

    /* Skip empty clusters (subtraction remnants) */

    const CaloClusterCellLink* theCellLink = thisEflowCaloObject->efRecCluster(0)->getCluster()->getCellLinks();
    CaloClusterCellLink::const_iterator it=theCellLink->begin();
    CaloClusterCellLink::const_iterator it_e=theCellLink->end();

    int numCell = 0;
    for (;it != it_e ; ++it) numCell++;

    if (0 == numCell){
      continue;
    }

    m_clustersToConsider.push_back(thisEflowCaloObject->efRecCluster(0));
  }

  std::sort(m_clustersToConsider.begin(),m_clustersToConsider.end(),eflowRecCluster::SortDescendingPt());
}

void eflowRecoverSplitShowersTool::getTracksToRecover() {

  m_tracksToRecover.clear();

  eflowCaloObjectContainer::iterator  itEFCalObject = m_eflowCaloObjectContainer->begin();
  eflowCaloObjectContainer::iterator endEFCalObject = m_eflowCaloObjectContainer->end();
  for (; itEFCalObject != endEFCalObject; ++itEFCalObject) {
    eflowCaloObject* thisEflowCaloObject = *itEFCalObject;

    /* Skip isolated tracks if flag set */
    if (thisEflowCaloObject->nClusters() == 0 && !m_recoverIsolatedTracks) {
      unsigned int nTrk = thisEflowCaloObject->nTracks();
      /* But make sure we get eflowObjects from them
       * TODO: replace this mechanism by something better */
      for (unsigned int iTrk = 0; iTrk < nTrk; ++iTrk) {
        thisEflowCaloObject->efRecTrack(iTrk)->setSubtracted();
      }
      continue;
    }

    /* Add all tracks on the eflowCaloObject that haven't been subtracted yet*/
    unsigned int nTracks = thisEflowCaloObject->nTracks();
    for (unsigned int iTrack = 0; iTrack < nTracks; ++iTrack){
      if (thisEflowCaloObject->efRecTrack(iTrack)->isSubtracted()){
        continue;
      }
      m_tracksToRecover.push_back(thisEflowCaloObject->efRecTrack(iTrack));
    }
  }

  std::sort(m_tracksToRecover.begin(),m_tracksToRecover.end(),eflowRecTrack::SortDescendingPt());

}

void eflowRecoverSplitShowersTool::performRecovery() {
  std::vector<eflowRecTrack*>::iterator itEfRecTrack = m_tracksToRecover.begin();
  std::vector<eflowRecTrack*>::iterator endEfRecTrack = m_tracksToRecover.end();
  for (; itEfRecTrack != endEfRecTrack; ++itEfRecTrack) {

    eflowRecTrack* thisEfRecTrack = *itEfRecTrack;

    /* No point to do anything if bin does not exist */
    if (!thisEfRecTrack->hasBin()) { continue; }

    if (1 == m_debug) {
      const xAOD::TrackParticle* track = thisEfRecTrack->getTrack();
      std::cout << "Recovering charged EFO with e,eta and phi " << track->e() << ", "
                << track->eta() << " and " << track->phi() << std::endl;
    }

    /* Get list of matched clusters */
    std::vector<eflowRecCluster*> matchedClusters = m_matchingTool->allMatches(thisEfRecTrack, m_clustersToConsider);
    m_nTrackClusterMatches += matchedClusters.size();

    /* Subtract the track from all matched clusters */
    subtractTrackFromClusters(thisEfRecTrack, matchedClusters);
    thisEfRecTrack->setSubtracted();
 }
}

void eflowRecoverSplitShowersTool::subtractTrackFromClusters(eflowRecTrack* efRecTrack,
                                                             std::vector<eflowRecCluster*> matchedClusters) {


  const eflowTrackCaloPoints& trackCalo = efRecTrack->getTrackCaloPoints();
  double trackE = efRecTrack->getTrack()->e();

  /* Determine the LFI */
  m_integrator->measureNewClus(matchedClusters, efRecTrack);
  eflowFirstIntENUM j1st = m_integrator->getFirstIntLayer();

  std::vector<xAOD::CaloCluster*> clusterSubtractionList;
  std::vector<eflowRecCluster*>::const_iterator  itCluster = matchedClusters.begin();
  std::vector<eflowRecCluster*>::const_iterator endCluster = matchedClusters.end();
  for (; itCluster != endCluster; ++itCluster) {
    clusterSubtractionList.push_back((*itCluster)->getClusterForModification(eflowCaloObject::getClusterContainerPtr()));
  }

  /* Get parameters for this eta, j1st */
  eflowCellSubtractionManager ranking;
  m_binnedParameters->getOrdering(ranking, trackE, trackCalo.getEM1eta(), j1st);

  const double eExpect = ranking.fudgeMean() * trackE;
  const double sigmaEExpect = fabs(ranking.fudgeStdDev()*trackE);

  if (1 == m_debug) {
    std::cout << "Expected mean and sigma are " << eExpect << " and " << sigmaEExpect << std::endl;
    std::cout << "j1st is " << j1st << std::endl;
    printClusterList(clusterSubtractionList, "Before subtraction");
  }

  /* Check if we can annihilate right away */
  if ( getSumEnergy(clusterSubtractionList) - eExpect < m_subtractionSigmaCut * sigmaEExpect ){
    clearClusters(clusterSubtractionList);
    if (1 == m_debug) { printClusterList(clusterSubtractionList, "After subtraction"); }
    return;
  }

  /* Make ordered cell list */
  eflowCellList orderedCells;
  makeOrderedCellList(trackCalo, clusterSubtractionList, orderedCells);

  /* Do subtraction of cells */
  eflowCellSubtractionFacilitator facilitator;
  facilitator.subtractCells(ranking,trackE , clusterSubtractionList, orderedCells);

  if (1 == m_debug) { printClusterList(clusterSubtractionList, "After subtraction"); }

  /* Annihilate the cluster(s) if the remnant is small (i.e. below k*sigma) */
  if ( getSumEnergy(clusterSubtractionList) < m_subtractionSigmaCut * sigmaEExpect ){
    clearClusters(clusterSubtractionList);
    orderedCells.eraseList();
  }

}

double eflowRecoverSplitShowersTool::getSumEnergy(const std::vector<xAOD::CaloCluster*>& clusters) {
  double result = 0.0;
  std::vector<xAOD::CaloCluster*>::const_iterator  itCluster = clusters.begin();
  std::vector<xAOD::CaloCluster*>::const_iterator endCluster = clusters.end();
  for (; itCluster != endCluster; ++itCluster) {
    result += (*itCluster)->e();
  }
  return result;
}

void eflowRecoverSplitShowersTool::clearClusters(std::vector<xAOD::CaloCluster*>& clusters) {
  std::vector<xAOD::CaloCluster*>::iterator  itCluster = clusters.begin();
  std::vector<xAOD::CaloCluster*>::iterator endCluster = clusters.end();
  for (; itCluster != endCluster; ++itCluster) {


    const CaloClusterCellLink* theCellLink = (*itCluster)->getCellLinks();
    CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);

    CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
    CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();
    
    for (; theFirstCell != theLastCell; ++theFirstCell) theCellLink_nonConst->removeCell(theFirstCell);
    
    (*itCluster)->setE(0.0);
    (*itCluster)->setRawE(0.0);
    CaloClusterKineHelper::calculateKine(*itCluster,true, true);
  }
}

void eflowRecoverSplitShowersTool::makeOrderedCellList(const eflowTrackCaloPoints& trackCalo,
                                                   const std::vector<xAOD::CaloCluster*>& clusters,
                                                   eflowCellList& orderedCells) {
  orderedCells.setNewExtrapolatedTrack(trackCalo);
  std::vector<xAOD::CaloCluster*>::const_iterator  itMatchedCluster = clusters.begin();
  std::vector<xAOD::CaloCluster*>::const_iterator endMatchedCluster = clusters.end();
  for (int countMatchedClusters = 0; itMatchedCluster != endMatchedCluster ; ++itMatchedCluster, ++countMatchedClusters){
    xAOD::CaloCluster *thisCluster = *itMatchedCluster;

    const CaloClusterCellLink* theCellLink = thisCluster->getCellLinks();
    CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
    CaloClusterCellLink::const_iterator lastCell = theCellLink->end();

    for (; firstCell != lastCell; firstCell++) {
      std::pair<CaloCell*,int> myPair(const_cast<CaloCell*>(*firstCell),countMatchedClusters);
      orderedCells.addCell(myPair);
    }//loop over cells in cluster

  }
}

void eflowRecoverSplitShowersTool::printClusterList(std::vector<xAOD::CaloCluster*>& clusters, std::string prefix) {
  std::vector<xAOD::CaloCluster*>::iterator firstMatchedClus = clusters.begin();
  std::vector<xAOD::CaloCluster*>::iterator lastMatchedClus = clusters.end();
  for (; firstMatchedClus != lastMatchedClus; ++firstMatchedClus) {
    std::cout << prefix << " have cluster with E, eta and phi of " << (*firstMatchedClus)->e()
              << ", " << (*firstMatchedClus)->eta() << " and " << (*firstMatchedClus)->phi()
              << std::endl;
  }
}
