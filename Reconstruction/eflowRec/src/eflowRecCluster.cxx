/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowRecCluster.cxx
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#include "eflowEvent/eflowCaloCluster.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"

eflowRecCluster::eflowRecCluster(const ElementLink<xAOD::CaloClusterContainer>& clusElementLink, eflowCaloObject* calob) :
    m_cluster(*clusElementLink), m_clusElementLink(clusElementLink), m_isTouchable(false), m_caloObject(calob) { }

void eflowRecCluster::replaceClusterByCopyInContainer(xAOD::CaloClusterContainer* container) {
  xAOD::CaloCluster* copiedCluster = new xAOD::CaloCluster(*m_cluster);
  container->push_back(copiedCluster);

  m_cluster = copiedCluster;

  m_clusElementLink = ElementLink<xAOD::CaloClusterContainer>();
  m_clusElementLink.toContainedElement(*container, copiedCluster);
}

xAOD::CaloCluster* eflowRecCluster::getClusterForModification(xAOD::CaloClusterContainer* container) {
  if (!m_isTouchable){
    replaceClusterByCopyInContainer(container);
    m_isTouchable = true;
  }
  return const_cast<xAOD::CaloCluster*>(getCluster());
}

bool eflowRecCluster::isEOverPFail(bool consistencySigmaCut, bool useGoldenMode) {

  double expectedEnergy = getSumExpectedEnergy();
  double clusterEnergy = m_cluster->e();

  if ((expectedEnergy == 0) && (clusterEnergy > 0)) {
    return false;
  }

  double sigma = sqrt(getVarianceOfSumExpectedEnergy());

  bool result = useGoldenMode ? fabs(clusterEnergy - expectedEnergy) > consistencySigmaCut*sigma
                              : clusterEnergy < expectedEnergy - consistencySigmaCut*sigma;
  return result;
}

double eflowRecCluster::getSumExpectedEnergy() {
  double expectedEnergy(0);
  int nTrk = getNTracks();
  for (int iTrk = 0; iTrk < nTrk; ++iTrk){
    expectedEnergy += m_trackMatches[iTrk]->getTrack()->getEExpect();
  }
  return expectedEnergy;
}
double eflowRecCluster::getVarianceOfSumExpectedEnergy() {
  double varianceExpectedEnergy(0);
  int nTrk = getNTracks();
  for (int iTrk = 0; iTrk < nTrk; ++iTrk){
    varianceExpectedEnergy += m_trackMatches[iTrk]->getTrack()->getVarEExpect();
  }
  return varianceExpectedEnergy;
}
