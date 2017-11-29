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
  m_clusterId(-1), m_cluster(*clusElementLink), m_clusElementLink(clusElementLink), m_isTouchable(false), m_type(0), m_matchCluster(nullptr) {
  m_matchCluster = std::make_unique<eflowMatchCluster>(this);
}

eflowRecCluster::eflowRecCluster(const eflowRecCluster& originalEflowRecCluster) {
  m_clusterId = originalEflowRecCluster.m_clusterId;
  m_cluster = originalEflowRecCluster.m_cluster;
  m_clusElementLink = originalEflowRecCluster.m_clusElementLink;
  m_isTouchable = originalEflowRecCluster.m_isTouchable;
  m_type = originalEflowRecCluster.m_type;
  m_matchCluster = std::make_unique<eflowMatchCluster>(this);
}

eflowRecCluster& eflowRecCluster::operator=(const eflowRecCluster& originalEflowRecCluster) {
  if (this == &originalEflowRecCluster) return *this;
  //if not assigning to self, then we copy the data to the new object
  else{
    m_cluster = originalEflowRecCluster.m_cluster;
    m_clusElementLink = originalEflowRecCluster.m_clusElementLink;
    m_isTouchable = originalEflowRecCluster.m_isTouchable;
    m_matchCluster = std::make_unique<eflowMatchCluster>(this);
    return *this;
  }//if not assigning to self, then we have copied the data to the new object
}

eflowRecCluster::~eflowRecCluster() { }

void eflowRecCluster::replaceClusterByCopyInContainer(xAOD::CaloClusterContainer* container) {
  std::unique_ptr<xAOD::CaloCluster> copiedCluster = std::make_unique<xAOD::CaloCluster>();
  container->push_back(std::move(copiedCluster));
  const CaloClusterCellLink* theOldCellLinks = m_cluster->getCellLinks();

  xAOD::CaloCluster* tempPtr = container->back();
  //xAOD::CaloCluster will take ownwership of this object, so we don't need to delete newLinks ourselves.
  CaloClusterCellLink *newLinks = new CaloClusterCellLink(*theOldCellLinks);
  tempPtr->addCellLink(newLinks);
  tempPtr->setClusterSize(xAOD::CaloCluster::Topo_420);
  CaloClusterKineHelper::calculateKine(tempPtr,true,true);

  tempPtr->setRawE(tempPtr->calE());
  tempPtr->setRawEta(tempPtr->calEta());
  tempPtr->setRawPhi(tempPtr->calPhi());
  tempPtr->setRawM(tempPtr->calM());

  //Cannot set m_cluster to copiedCluster - std::move makes copiedCluster a nullptr. Hence must access from
  //back of container, we have just pushed into.
  m_cluster = container->back();
  m_clusElementLink = ElementLink<xAOD::CaloClusterContainer>();
  m_clusElementLink.toContainedElement(*container,container->back());
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
