/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatchPositions.cxx
 *
 *  Created on: 25.03.2014
 *      Author: tlodd
 */

#include <cassert>
#include <iostream>

#include "eflowRec/PFMatchPositions.h"

namespace PFMatch {

/* Track position providers */

EtaPhi* TrackEtaPhiInFixedLayersProvider::getPosition(const ITrack* track) {
  eflowEtaPhiPosition etaphi = track->etaPhiInLayer(m_barrelLayer);
  if (etaphi.getEta() == -999.){
    etaphi = track->etaPhiInLayer(m_endcapLayer);
  }
  //*m_position = EtaPhi(etaphi);
  m_position = std::make_unique<EtaPhi>(etaphi);
  return m_position.get();
}


/* Cluster position providers */

EtaPhi* ClusterPlainEtaPhiProvider::getPosition(const ICluster* cluster) {
  //*m_position = eflowEtaPhiPosition(cluster->eta(), cluster->phi());
  //m_position = std::make_unique<eflowEtaPhiPosition>(cluster->eta(), cluster->phi());
  eflowEtaPhiPosition etaphi(cluster->eta(), cluster->phi());
  m_position = std::make_unique<EtaPhi>(etaphi);
  return m_position.get();
}

const double ClusterGeometricalCenterProvider::m_etaPhiLowerLimit(0.0025);

EtaPhiWithVariance* ClusterGeometricalCenterProvider::getPosition(const ICluster* cluster) {

  /* Check the status to make sure this function only execute once since it is expensive. */
  if(cluster->calVarianceStatus()) {
    //*m_position = EtaPhiWithVariance(eflowEtaPhiPosition(cluster->etaMean(), cluster->phiMean()), cluster->etaVariance(), cluster->phiVariance());
    m_position = std::make_unique<EtaPhiWithVariance>(eflowEtaPhiPosition(cluster->etaMean(), cluster->phiMean()), cluster->etaVariance(), cluster->phiVariance());
    return m_position.get();
  }
  cluster->setCalVarianceStatus();

  unsigned int nCells = cluster->nCells();

  /* Catch empty clusters */
  if (nCells == 0){
    cluster->etaVariance(m_etaPhiLowerLimit);
    cluster->phiVariance(m_etaPhiLowerLimit);
    //*m_position = EtaPhiWithVariance(eflowEtaPhiPosition(cluster->eta(), cluster->phi()), cluster->etaVariance(), cluster->phiVariance());
    m_position = std::make_unique<EtaPhiWithVariance>(eflowEtaPhiPosition(cluster->eta(), cluster->phi()), cluster->etaVariance(), cluster->phiVariance());
    return m_position.get();
  }
  assert(nCells > 0);

  /* Deal with 1 cell cluster */ 
  if (1 == nCells){ 
    cluster->etaVariance(m_etaPhiLowerLimit);
    cluster->phiVariance(m_etaPhiLowerLimit);
    //*m_position = EtaPhiWithVariance(eflowEtaPhiPosition(cluster->eta(), cluster->phi()), cluster->etaVariance(), cluster->phiVariance());
    m_position = std::make_unique<EtaPhiWithVariance>(eflowEtaPhiPosition(cluster->eta(), cluster->phi()), cluster->etaVariance(), cluster->phiVariance());
    return m_position.get();
  } 


  /* Sum eta, eta^2, phi and phi^2 of all cells */
  double sumeta = 0;
  double sumeta2 = 0;
  double sumphi = 0;
  double sumphi2 = 0;
  double thisCellPhi;
  const std::vector<double>& cellEta = cluster->cellEta();
  const std::vector<double>& cellPhi = cluster->cellPhi();
  for(unsigned int iCell = 0; iCell < nCells; ++iCell) {
    sumeta  += cellEta[iCell];
    sumeta2 += cellEta[iCell]*cellEta[iCell];
    thisCellPhi = eflowAzimuth(cellPhi[iCell]).cycle(cluster->phi());
    sumphi  += thisCellPhi;
    sumphi2 += thisCellPhi*thisCellPhi;
  }

  /* Calculate mean eta and phi */
  double etaMean = sumeta/((double)nCells);
  double phiMean = sumphi/((double)nCells);

  /* Calculate variance of eta and phi (but don't let them go below the lower limit) */
  double varianceCorrection = (double)nCells / (double)(nCells-1);
  double etaVariance = std::max(m_etaPhiLowerLimit, (sumeta2/(double)nCells - etaMean*etaMean) * varianceCorrection);
  double phiVariance = std::max(m_etaPhiLowerLimit, (sumphi2/(double)nCells - phiMean*phiMean) * varianceCorrection);

  cluster->etaMean(etaMean);
  cluster->phiMean(phiMean);
  cluster->etaVariance(etaVariance);
  cluster->phiVariance(phiVariance);
  //*m_position = EtaPhiWithVariance(eflowEtaPhiPosition(etaMean, phiMean), cluster->etaVariance(), cluster->phiVariance());
  m_position = std::make_unique<EtaPhiWithVariance>(eflowEtaPhiPosition(etaMean, phiMean), cluster->etaVariance(), cluster->phiVariance());
  return m_position.get();
}

}
