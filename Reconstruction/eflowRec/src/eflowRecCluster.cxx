/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

eflowRecCluster::eflowRecCluster(const ElementLink<xAOD::CaloClusterContainer>& clusElementLink, xAOD::CaloClusterContainer& newClusContainer) :
  m_clusterId(-1), m_originalClusElementLink(clusElementLink), m_isTouchable(false),m_calorimeterType(UNASSIGNED) , m_matchCluster(nullptr) {
  const xAOD::CaloCluster* originalCluster = *clusElementLink;
  m_cluster = new xAOD::CaloCluster();
  newClusContainer.push_back(m_cluster);
  m_clusElementLink = ElementLink<xAOD::CaloClusterContainer>(newClusContainer,newClusContainer.size()-1);

  const CaloClusterCellLink* theOldCellLinks = originalCluster->getCellLinks();

  CaloClusterCellLink *newLinks = new CaloClusterCellLink(*theOldCellLinks);
  m_cluster->addCellLink(newLinks);
  m_cluster->setClusterSize(xAOD::CaloCluster::Topo_420);
  CaloClusterKineHelper::calculateKine(m_cluster,true,true);

  m_cluster->setRawE(m_cluster->calE());
  m_cluster->setRawEta(m_cluster->calEta());
  m_cluster->setRawPhi(m_cluster->calPhi());
  m_cluster->setRawM(m_cluster->calM());

  m_matchCluster = std::make_unique<eflowMatchCluster>(this);
}

eflowRecCluster::eflowRecCluster(const eflowRecCluster& originalEflowRecCluster) {
  m_clusterId = originalEflowRecCluster.m_clusterId;
  m_cluster = originalEflowRecCluster.m_cluster;
  m_clusElementLink = originalEflowRecCluster.m_clusElementLink;
  m_originalClusElementLink = originalEflowRecCluster.m_originalClusElementLink;
  m_isTouchable = originalEflowRecCluster.m_isTouchable;
  m_calorimeterType = originalEflowRecCluster.m_calorimeterType;
  m_matchCluster = std::make_unique<eflowMatchCluster>(this);
}

eflowRecCluster& eflowRecCluster::operator=(const eflowRecCluster& originalEflowRecCluster) {
  if (this == &originalEflowRecCluster) return *this;
  //if not assigning to self, then we copy the data to the new object
  else{
    m_cluster = originalEflowRecCluster.m_cluster;
    m_clusElementLink = originalEflowRecCluster.m_clusElementLink;
    m_originalClusElementLink = originalEflowRecCluster.m_originalClusElementLink;
    m_isTouchable = originalEflowRecCluster.m_isTouchable;
    m_calorimeterType = originalEflowRecCluster.m_calorimeterType;
    m_matchCluster = std::make_unique<eflowMatchCluster>(this);
    return *this;
  }//if not assigning to self, then we have copied the data to the new object
}

eflowRecCluster::~eflowRecCluster() { }

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
  if(m_calorimeterType!=UNASSIGNED) {
    return m_calorimeterType;
  }
  CaloClusterKineHelper::calculateKine(m_cluster, true, true);

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
  double ratioFCAL = (FCAL_E + MiniFCAL_E)/totalEnergy;

  if(ratioEM > 0.5) {
    m_calorimeterType = ECAL;
  } else if (ratioHCAL > 0.5) {
    m_calorimeterType = HCAL;
  } else if (ratioFCAL > 0.5) {
    m_calorimeterType = FCAL;
  } else {
    m_calorimeterType = UNKNOWN;
  }

  assert(m_calorimeterType!=UNASSIGNED);
  return m_calorimeterType;

}
