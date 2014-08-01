/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     eflowCellLevelSubtractionTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  25th January, 2005

********************************************************************/

#include "eflowRec/eflowCellLevelSubtractionTool.h"

#include "eflowRec/eflowBinnedParameters.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowDatabase.h"
#include "eflowRec/eflowDepthCalculator.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowCellEOverPTool.h"
#include "eflowRec/cycle.h"
#include "eflowRec/phicorr.h"
#include "eflowRec/eflowCellSubtractionManager.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"

#include "eflowEvent/eflowCaloCluster.h"

#include "CaloEvent/CaloCluster.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "eflowRec/eflowCaloObject.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ListItem.h"

#include <iostream>
#include <cmath>

eflowCellLevelSubtractionTool::eflowCellLevelSubtractionTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_rCell(0.75),
  m_subtractionSigmaCut(1.28),
  m_consistencySigmaCut(1.0),
  m_calcEOverP(false),
  m_goldenModeString("")
{ 
  declareProperty("SubtractionSigmaCut",m_subtractionSigmaCut);
  declareProperty("ConsistencySigmaCut",m_consistencySigmaCut);
  declareProperty("CalcEOverP",m_calcEOverP,"Whether to disable energy flow");
  declareProperty("goldenModeString",m_goldenModeString,"run in golden match mode only?");
}

StatusCode eflowCellLevelSubtractionTool::initialize(){

  return StatusCode::SUCCESS;

}

void eflowCellLevelSubtractionTool::execute(eflowCaloObjectContainer* theEflowCaloObjectContainer) {
  msg(MSG::DEBUG) << "Running eflowObjectCreatorTool on the eflowCaloObjectContainer " << endreq;

  unsigned int nEFCaloObs = theEflowCaloObjectContainer->size();
  for (unsigned int iEFCalOb = 0; iEFCalOb < nEFCaloObs; ++iEFCalOb){
    eflowCaloObject* thisEflowCaloObject = theEflowCaloObjectContainer->at(iEFCalOb);
    if (execute(thisEflowCaloObject).isFailure()){
      msg(MSG::WARNING) << "Execute failed for one eflowCaloObject. " << endreq;
      continue;
    }
  }
}

StatusCode eflowCellLevelSubtractionTool::execute(eflowCaloObject *energyFlowCaloObject) {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Executing eflowCellLevelSubtractionTool" << endreq;

  /* Check e/p mode */
  if (m_calcEOverP) { return StatusCode::SUCCESS; }

  unsigned int nClusters= energyFlowCaloObject->nClusters();
  if (energyFlowCaloObject->nClusters() > 1){
    msg(MSG::WARNING) << "eflowCaloObject has "<< energyFlowCaloObject->nClusters() <<" clusters. Will only process the first one!" << endreq;
    nClusters = 1;
  }

  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){
    eflowRecCluster* thisEfRecCluster = energyFlowCaloObject->efRecCluster(iCluster);

    /* Check e/p */
    if ( thisEfRecCluster->isEOverPFail(m_consistencySigmaCut, runInGoldenMode()) ||
        (runInGoldenMode() && thisEfRecCluster->getNTracks() > 1) ) {
      continue;
    }

    /* Do subtraction */
    subtractTracksFromCluster(thisEfRecCluster);

    /* Flag tracks as subtracted */
    const std::vector<eflowTrackClusterLink*>& matchedTrackList = thisEfRecCluster->getTrackMatches();
    unsigned int nTrackMatches = thisEfRecCluster->getNTracks();
    for (unsigned int iTrack = 0; iTrack < nTrackMatches; ++iTrack){
      matchedTrackList[iTrack]->getTrack()->setSubtracted();
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode eflowCellLevelSubtractionTool::finalize(){

  return StatusCode::SUCCESS;
}

void eflowCellLevelSubtractionTool::subtractTracksFromCluster(eflowRecCluster* thisEfRecCluster) {
  int nTrackMatches = thisEfRecCluster->getNTracks();

  if (nTrackMatches == 0 || runInGoldenMode()){ return; }

  xAOD::CaloCluster* cluster = thisEfRecCluster->getClusterForModification(eflowCaloObject::getClusterContainerPtr());

  /* Check if we can annihilate right away */
  double sumVar = thisEfRecCluster->getVarianceOfSumExpectedEnergy();
  if ( cluster->e() - thisEfRecCluster->getSumExpectedEnergy() < m_subtractionSigmaCut * sqrt(sumVar) ) {
    annihilateCluster(cluster);
    return;
  }

  /* Otherwise need to subtract cell by cell */
  const std::vector<eflowTrackClusterLink*>& matchedTrackList = thisEfRecCluster->getTrackMatches();
  for (int iTrack = 0; iTrack < nTrackMatches; ++iTrack){

    eflowRecTrack* efRecTrack = matchedTrackList[iTrack]->getTrack();

    /* Can't subtract without e/p */
    if (!efRecTrack->hasBin()){ continue; }

    /* Make ordered cell list */
    /* (Invokes newCluster() on orderedCells, than adds all the cells in tracksClus) */
    makeOrderedCellList(efRecTrack->getTrackCaloPoints(), cluster);

    /* Get the cellSubtractionManager from the eflowCaloObject */
    eflowCellSubtractionManager& ranking = efRecTrack->getCellSubtractionManager();

    const double trackE = efRecTrack->getTrack()->e();

    eflowCellSubtractionFacilitator facilitator;
    facilitator.subtractCells(ranking,trackE , cluster, m_orderedCells);

  }//end of track loop

  if ( cluster->e() < m_subtractionSigmaCut * sqrt(sumVar) ) {
    annihilateCluster(cluster);
    m_orderedCells.eraseList();
  }

}

void eflowCellLevelSubtractionTool::annihilateCluster(xAOD::CaloCluster* cluster) {

  const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
  CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);

  CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
  CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();
  
  for (; theFirstCell != theLastCell; ++theFirstCell) theCellLink_nonConst->removeCell(theFirstCell);

  cluster->setE(0.0);
  cluster->setRawE(0.0);
  CaloClusterKineHelper::calculateKine(cluster,true, true);

}

void eflowCellLevelSubtractionTool::makeOrderedCellList(const eflowTrackCaloPoints& trackCalo,
                                                       xAOD::CaloCluster* cluster) {
  
  m_orderedCells.setNewExtrapolatedTrack(trackCalo);

  const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
  CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
  CaloClusterCellLink::const_iterator lastCell = theCellLink->end();

  for (; firstCell != lastCell; firstCell++) {
    std::pair<CaloCell*,int> myPair(const_cast<CaloCell*>(*firstCell),0);//0 is dummy. dont need it
    m_orderedCells.addCell(myPair);
  }
}
