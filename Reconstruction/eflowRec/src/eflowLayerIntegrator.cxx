/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowLayerIntegrator.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowCellIntegrator.h"
#include "eflowRec/eflowDepthCalculator.h"
#include "eflowRec/cycle.h"
#include "eflowRec/LegendreWeights.h"
#include "eflowRec/eflowDatabase.h"
#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowCaloRegions.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloEvent/CaloCell.h"

eflowLayerIntegrator::eflowLayerIntegrator(double stdDev, double error, double rMaxOverStdDev) :
    m_rMax(rMaxOverStdDev * stdDev),
    m_allClustersIntegral(eflowCalo::nRegions, 0.0),
    m_nUnitCellPerWindowOverCellEtaPhiArea(eflowCalo::nRegions),
    m_integrator(std::make_unique<eflowCellIntegrator<0> >(stdDev, error)),
    m_integratorLookup(std::make_unique<eflowCellIntegrator<1> >(stdDev, error)) {
  eflowDatabase database;

  /* Set up density conversion factors */
  double emX0PerUnitLengthToTheMinus3  = pow(database.getEmX0PerUnitLength(),  -3.0);
  double hadX0PerUnitLengthToTheMinus3 = pow(database.getHadX0PerUnitLength(), -3.0);
  for (int i = 0; i < eflowCalo::nRegions; i++) {
    eflowCaloENUM layer = (eflowCaloENUM)(i);
    m_densityConversion[i] = (layer >= eflowCalo::EMB1 && layer <= eflowCalo::EME3) ?
                             emX0PerUnitLengthToTheMinus3 :
                             hadX0PerUnitLengthToTheMinus3;
  }

  /* More setup */
  double nUnitCellsPerWindow = M_PI * stdDev * stdDev / (database.getEtaUnit() * database.getPhiUnit());
  std::vector<double> cellEtaWidth = database.getCellEtaWidth();
  std::vector<double> cellPhiWidth = database.getCellPhiWidth();
  for (int i = 0; i < eflowCalo::nRegions; i++){
    m_nUnitCellPerWindowOverCellEtaPhiArea[i] =
        nUnitCellsPerWindow / (cellEtaWidth[i] * cellPhiWidth[i]);
  }
}

eflowLayerIntegrator::eflowLayerIntegrator(const eflowLayerIntegrator& originalEflowLayerIntegrator){
  m_rMax = originalEflowLayerIntegrator.m_rMax;
  m_allClustersIntegral =  originalEflowLayerIntegrator.m_allClustersIntegral;
  m_nUnitCellPerWindowOverCellEtaPhiArea = originalEflowLayerIntegrator.m_nUnitCellPerWindowOverCellEtaPhiArea;
  m_integrator = std::make_unique<eflowCellIntegrator<0> >(*originalEflowLayerIntegrator.m_integrator);
  m_integratorLookup = std::make_unique<eflowCellIntegrator<1> >(*originalEflowLayerIntegrator.m_integratorLookup);

  for (int i = 0; i < eflowCalo::nRegions; i++){
    m_densityConversion[i] = originalEflowLayerIntegrator.m_densityConversion[i];
    m_nUnitCellPerWindowOverCellEtaPhiArea[i] = originalEflowLayerIntegrator.m_nUnitCellPerWindowOverCellEtaPhiArea[i];
  }
}

eflowLayerIntegrator& eflowLayerIntegrator::operator=(const eflowLayerIntegrator& originalEflowLayerIntegrator){
  if (this == &originalEflowLayerIntegrator) return *this;
  //if not assigning to self, then we copy the data to the new object
  else {
    m_rMax = originalEflowLayerIntegrator.m_rMax;
    m_allClustersIntegral =  originalEflowLayerIntegrator.m_allClustersIntegral;
    m_nUnitCellPerWindowOverCellEtaPhiArea = originalEflowLayerIntegrator.m_nUnitCellPerWindowOverCellEtaPhiArea;
    m_integrator = std::make_unique<eflowCellIntegrator<0> >(*originalEflowLayerIntegrator.m_integrator);
    m_integratorLookup = std::make_unique<eflowCellIntegrator<1> >(*originalEflowLayerIntegrator.m_integratorLookup);

    for (int i = 0; i < eflowCalo::nRegions; i++){
      m_densityConversion[i] = originalEflowLayerIntegrator.m_densityConversion[i];
      m_nUnitCellPerWindowOverCellEtaPhiArea[i] = originalEflowLayerIntegrator.m_nUnitCellPerWindowOverCellEtaPhiArea[i];
    }
    return *this;
  }//if not assigning to self, then we have copied the data to the new object
}

eflowLayerIntegrator::~eflowLayerIntegrator() {
}

void eflowLayerIntegrator::resetAllClustersIntegralForNewTrack(const eflowTrackCaloPoints& trackCalo) {
  for (int iLayer = 0; iLayer < eflowCalo::nRegions; iLayer++) {
    m_allClustersIntegral[iLayer] = trackCalo.haveLayer((eflowCaloENUM)iLayer) ? 0.0 : eflowTrackCaloPoints::defaultEta();
  }

  /* Calculate the caloDepthArray */
  double em2Eta = trackCalo.getEM2eta();
  if ( fabs(em2Eta) > 2.5 ) { em2Eta = 2.49; }   //sometimes track extrapolator returns e.g. 2.51 for em2Eta, which causes depth array to be filled with zeroes.
  m_caloModel.calcDepthArray(em2Eta, 1.0e-4);
}

void eflowLayerIntegrator::addToAllClustersIntegral(const std::vector<double>& clusterIntegral) {
  assert(clusterIntegral.size() == m_allClustersIntegral.size());

  if (clusterIntegral.size() != m_allClustersIntegral.size()){
    std::cerr << " eflowLayerIntegrator ERROR: cluster integral sizes do not match" << std::endl;
    return;
  }

  for (int i = 0; i < eflowCalo::nRegions; i++) {
    if (m_allClustersIntegral[i] != eflowTrackCaloPoints::defaultEta()){
      m_allClustersIntegral[i] += clusterIntegral[i];
    }
  }
}

void eflowLayerIntegrator::measureNewClus(std::vector<xAOD::CaloCluster*> clusVec, const eflowTrackCaloPoints& trackCalo) {
  resetAllClustersIntegralForNewTrack(trackCalo);

  std::vector<xAOD::CaloCluster*>::iterator itCluster    = clusVec.begin();
  std::vector<xAOD::CaloCluster*>::iterator itClusterEnd = clusVec.end();
  for (; itCluster != itClusterEnd; itCluster++){
    measureCluster(*itCluster, trackCalo);
  }
}

void eflowLayerIntegrator::measureNewClus(std::vector<eflowRecCluster*> efRecClusters, eflowRecTrack* track) {
  resetAllClustersIntegralForNewTrack(track->getTrackCaloPoints());

  std::vector<eflowRecCluster*>::iterator  itCluster = efRecClusters.begin();
  std::vector<eflowRecCluster*>::iterator endCluster = efRecClusters.end();
  for (; itCluster != endCluster; itCluster++){
    measureCluster(eflowTrackClusterLink::getInstance(track, *itCluster));
  }

}

void eflowLayerIntegrator::measureNewClus(eflowTrackClusterLink* trackClusterLink) {
  resetAllClustersIntegralForNewTrack(trackClusterLink->getTrack()->getTrackCaloPoints());

  measureCluster(trackClusterLink);
}

void eflowLayerIntegrator::measureNewClus(const xAOD::CaloCluster* clus, const eflowTrackCaloPoints& trackCalo) {
  resetAllClustersIntegralForNewTrack(trackCalo);

  measureCluster(clus, trackCalo);
}

void eflowLayerIntegrator::measureCluster(eflowTrackClusterLink* trackClusterLink) {
  if (trackClusterLink->getClusterIntegral().empty()){
    /* The track-cluster pair hasn't been integrated yet. Integrate as usual and store the results */
    measureCluster(trackClusterLink->getCluster()->getCluster(), trackClusterLink->getTrack()->getTrackCaloPoints());
    trackClusterLink->setClusterIntegral(m_singleClusterIntegral);
    if (trackClusterLink->getTrack()->getCaloDepthArray().empty()) {
      trackClusterLink->getTrack()->setCaloDepthArray(m_caloModel.getDepthArray());
    }
  } else {
    /* The track-cluster pair has already been integrated. Take integral from the TrackClusterLink and add to total integral */
    addToAllClustersIntegral(trackClusterLink->getClusterIntegral());
  }
}

void eflowLayerIntegrator::measureCluster(const xAOD::CaloCluster* clus, const eflowTrackCaloPoints& trackCalo) {
  m_singleClusterIntegral.assign(eflowCalo::nRegions, 0.0);

  const CaloClusterCellLink* theCellLink = clus->getCellLinks();

  if (theCellLink){
  
    CaloClusterCellLink::const_iterator itCell = theCellLink->begin();
    CaloClusterCellLink::const_iterator itCellEnd = theCellLink->end();

    for (; itCell != itCellEnd; ++itCell) {
      measureCell(*itCell, trackCalo);
    }
  }//if valid cell link

  addToAllClustersIntegral(m_singleClusterIntegral);
}

void eflowLayerIntegrator::measureCell(const CaloCell* cell, const eflowTrackCaloPoints& trackCalo) {
  const CaloDetDescrElement* caloDetDescrElement = cell->caloDDE();
  if (!caloDetDescrElement) return;

  eflowCaloENUM layer = eflowCalo::translateSampl(caloDetDescrElement->getSampling());
  if (eflowCalo::Unknown == layer) { return; }

  const double extrapTrackEta = trackCalo.getEta(layer);
  const double extrapTrackPhi = trackCalo.getPhi(layer);
  if (extrapTrackEta == eflowTrackCaloPoints::defaultEta() ||
      extrapTrackPhi == eflowTrackCaloPoints::defaultPhi()) {
    return;
  }

  const double etaWidth = caloDetDescrElement->deta();
  const double phiWidth = caloDetDescrElement->dphi();

  const double dEta = cell->eta() - extrapTrackEta;
  const double dPhi = cycle(cell->phi(), extrapTrackPhi) - extrapTrackPhi;
  const double dr = sqrt(dEta * dEta + dPhi * dPhi);

  if ( fabs(dr - std::max(etaWidth, phiWidth)) <= m_rMax || dr <= m_rMax ) {

    eflowRange etaRange(dEta - etaWidth/2.0, dEta + etaWidth/2.0);
    eflowRange phiRange(dPhi - phiWidth/2.0, dPhi+phiWidth/2.0);

//    const double weight = m_integrator->integrate(etaRange, phiRange);
    const double weight     = m_integratorLookup->integrate(etaRange, phiRange);

    m_singleClusterIntegral[layer] += weight * cell->energy() / caloDetDescrElement->volume();
  }
}

eflowFirstIntENUM eflowLayerIntegrator::getFirstIntLayer() const {

  const double* depthArray = m_caloModel.getDepthArray();

  double xPrev = 0.0;
  double yPrev = 0.0;
  int result = eflowCalo::Unknown;
  double maxGradient = -10.0;

  for (int layer = 0; layer < eflowCalo::nRegions; ++layer){
    if (m_allClustersIntegral[layer] == eflowTrackCaloPoints::defaultEta()) { continue; }

    double convertedDensity = m_allClustersIntegral[layer] * m_densityConversion[layer];
    eflowDepthLayerENUM depthLayer = eflowDepthCalculator::depthIndex((eflowCaloENUM)layer);

    const double dx = depthArray[depthLayer+1] - xPrev;
    const double dy = convertedDensity - yPrev;
    xPrev = depthArray[depthLayer+1];
    yPrev = convertedDensity;
    const double gradient  = dy / dx;

    if (gradient > maxGradient) {
      maxGradient = gradient;
      result = layer;
    }
  }

  return eflowFirstIntRegions::translateCalo((eflowCaloENUM)result);
}
