/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowCaloObject.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R.Tovey, M.Hodgkinson and T.Velz
CREATED:  22nd November, 2004

********************************************************************/

#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowRingSubtractionManager.h"

xAOD::CaloClusterContainer* eflowCaloObject::m_clusterContainerPtr = 0;
xAOD::CaloClusterAuxContainer* eflowCaloObject::m_clusterAuxContainerPtr = 0;

eflowCaloObject::~eflowCaloObject() { }


 double eflowCaloObject::getExpectedEnergy() {
  double expectedEnergy(0.0);
  for(unsigned iTrack=0; iTrack < nTracks(); ++iTrack) {
    expectedEnergy += efRecTrack(iTrack)->getEExpect();
  }
  return expectedEnergy;
}

double eflowCaloObject::getExpectedVariance() {
  double expectedVariance(0.0);
  for(unsigned iTrack=0; iTrack < nTracks(); ++iTrack) {
    expectedVariance += efRecTrack(iTrack)->getVarEExpect();
  }
  return expectedVariance;
}

double eflowCaloObject::getClusterEnergy() {
  double clusterEnergy(0.0);
  for(unsigned iCluster=0; iCluster < nClusters(); ++iCluster) {
    clusterEnergy  += efRecCluster(iCluster)->getCluster()->e();
  }
  return clusterEnergy;
}

void eflowCaloObject::simulateShower(eflowLayerIntegrator *integrator, eflowEEtaBinnedParameters* binnedParameters){

  for (unsigned int iTrack = 0; iTrack < m_eflowRecTracks.size(); ++iTrack) {

    eflowRecTrack* thisEfRecTrack = m_eflowRecTracks.at(iTrack);

    std::vector<eflowRecCluster*> matchedClusters;
    matchedClusters.clear();
    std::vector<eflowTrackClusterLink*> links = thisEfRecTrack->getClusterMatches();
    std::vector<eflowTrackClusterLink*>::iterator itLink = links.begin();
    std::vector<eflowTrackClusterLink*>::iterator endLink = links.end();
    for (; itLink != endLink; ++itLink) {
      matchedClusters.push_back((*itLink)->getCluster());
    }

    double trackEM1eta = thisEfRecTrack->getTrackCaloPoints().getEM1eta();
    double trackE = thisEfRecTrack->getTrack()->e();

    if (!binnedParameters->binExists(trackE, trackEM1eta)) {
      thisEfRecTrack->setHasBin(false);
      return;
    }

    /* Determine the LFI */
    integrator->measureNewClus(matchedClusters, thisEfRecTrack);
    eflowFirstIntENUM j1st = integrator->getFirstIntLayer();

    /* Get parameters for j1st */
    eflowRingSubtractionManager& cellSubtractionManager = thisEfRecTrack->getCellSubtractionManager();
    cellSubtractionManager.getOrdering(binnedParameters, trackE, trackEM1eta, j1st);


    /* Set expected energy in the eflowRecTrack object */
    const double eExpect = cellSubtractionManager.fudgeMean() * thisEfRecTrack->getTrack()->e();
    const double sigmaEExpect = fabs(cellSubtractionManager.fudgeStdDev() * thisEfRecTrack->getTrack()->e());
    thisEfRecTrack->setEExpect(eExpect, sigmaEExpect * sigmaEExpect);
  }
}

