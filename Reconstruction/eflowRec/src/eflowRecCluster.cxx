/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowRecCluster.cxx
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

eflowRecCluster::eflowRecCluster(const ElementLink<xAOD::CaloClusterContainer>& clusElementLink) :
  m_clusterId(-1), m_cluster(*clusElementLink), m_clusElementLink(clusElementLink), m_isTouchable(false), m_type(0), m_matchCluster(0) {
  m_matchCluster = new eflowMatchCluster(this);
}

eflowRecCluster::eflowRecCluster(const eflowRecCluster& anEFlowRecCluster) {
  m_clusterId = anEFlowRecCluster.m_clusterId;
  m_cluster = anEFlowRecCluster.m_cluster;
  m_clusElementLink = anEFlowRecCluster.m_clusElementLink;
  m_isTouchable = anEFlowRecCluster.m_isTouchable;
  m_type = anEFlowRecCluster.m_type;
  m_matchCluster = new eflowMatchCluster(this);
}

void eflowRecCluster::operator=(const eflowRecCluster& anEFlowRecCluster) {
  m_cluster = anEFlowRecCluster.m_cluster;
  m_clusElementLink = anEFlowRecCluster.m_clusElementLink;
  m_isTouchable = anEFlowRecCluster.m_isTouchable;
  m_matchCluster = new eflowMatchCluster(this);
}

eflowRecCluster::~eflowRecCluster() { delete m_matchCluster; }

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

int eflowRecCluster::getClusterType() {
  if(m_type!=0) return m_type;
  CaloClusterKineHelper::calculateKine(const_cast<xAOD::CaloCluster*>(m_cluster), true, true);

  double EMB_E = m_cluster->eSample(xAOD::CaloCluster::CaloSample::PreSamplerB)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::EMB1)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::EMB2)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::EMB3);
  double EME_E = m_cluster->eSample(xAOD::CaloCluster::CaloSample::PreSamplerE)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::EME1)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::EME2)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::EME3);
  double HEC_E = m_cluster->eSample(xAOD::CaloCluster::CaloSample::HEC0)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::HEC1)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::HEC2)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::HEC3);
  double Tile_E = m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar0)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar1)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileBar2)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap1)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap2)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileGap3)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt0)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt1)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::TileExt2);
  double FCAL_E = m_cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL0)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL1)
      + m_cluster->eSample(xAOD::CaloCluster::CaloSample::FCAL2);
  double MiniFCAL_E = m_cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL0)
          + m_cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL1)
          + m_cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL2)
          + m_cluster->eSample(xAOD::CaloCluster::CaloSample::MINIFCAL3);

  double totalEnergy = EMB_E + EME_E + HEC_E + Tile_E + FCAL_E + MiniFCAL_E;
  double ratioEM = (EMB_E+EME_E)/totalEnergy;
  double ratioHCAL = (HEC_E+Tile_E)/totalEnergy;

  if(ratioEM > 0.5) {
    m_type = 1;
  } else if (ratioHCAL > 0.5) {
    m_type = 2;
  } else {
    m_type = 3;
  }

  assert(m_type!=0);
  return m_type;

}
