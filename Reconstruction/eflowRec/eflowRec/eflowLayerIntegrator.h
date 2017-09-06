/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWLAYERINTEGRATOR_H
#define EFLOWLAYERINTEGRATOR_H

/********************************************************************

NAME:     eflowLayerIntegrator.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

#include <vector>
#include "eflowCaloRegions.h"
#include "eflowDepthCalculator.h"
#include "eflowRec/eflowUtil.h"
#include "xAODCaloEvent/CaloCluster.h"

class CaloCell;
class eflowRecCluster;
class eflowRecTrack;
class eflowTrackClusterLink;
class eflowTrackCaloPoints;

template <int expType> class eflowCellIntegrator;

class eflowLayerIntegrator {

 public:

  eflowLayerIntegrator(double stdDev, double error, double rMaxOverStdDev);
  eflowLayerIntegrator(const eflowLayerIntegrator& originalEflowLayerIntegrator);
  eflowLayerIntegrator&  operator=(const eflowLayerIntegrator& originalEflowLayerIntegrator);
  ~eflowLayerIntegrator();

  void measureNewClus(const xAOD::CaloCluster* clus, const eflowTrackCaloPoints& trackCalo);
  void measureNewClus(eflowTrackClusterLink* trackClusterLink);
  void measureNewClus(std::vector<eflowRecCluster*> efRecClusters, eflowRecTrack* track);
  void measureNewClus(std::vector<xAOD::CaloCluster*> clusVec, const eflowTrackCaloPoints& trackCalo);

  eflowFirstIntENUM getFirstIntLayer() const;

 private:
  void resetAllClustersIntegralForNewTrack(const eflowTrackCaloPoints& trackCalo);
  void addToAllClustersIntegral(const std::vector<double>& clusterIntegral);

  void measureCluster(eflowTrackClusterLink* trackClusterLink);
  void measureCluster(const xAOD::CaloCluster* clus, const eflowTrackCaloPoints& trackCalo);
  void measureCell(const CaloCell* cell, const eflowTrackCaloPoints& trackCalo);

  double m_rMax;

  std::vector<double> m_allClustersIntegral;
  std::vector<double> m_singleClusterIntegral;
  
  eflowDepthCalculator m_caloModel;

  double m_densityConversion[eflowCalo::nRegions];

  std::vector<double> m_nUnitCellPerWindowOverCellEtaPhiArea;

  std::unique_ptr<eflowCellIntegrator<0> > m_integrator;
  std::unique_ptr<eflowCellIntegrator<1> > m_integratorLookup;
};

#endif
