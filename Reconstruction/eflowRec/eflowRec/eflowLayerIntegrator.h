/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

/**
 This class calculates the LHED (Layer of Highest Energy Density) in a cluster or group of clusters. This is calculated using an integral over the eta-phi enegry density in each calorimeter layer using 2D Gaussian Weights, and makes use of eflowCellIntegrator to do the integral over each calorimeter cell.
*/
class eflowLayerIntegrator {

 public:

  eflowLayerIntegrator(double stdDev, double error, double rMaxOverStdDev, bool isHLLHC = false);
  eflowLayerIntegrator(const eflowLayerIntegrator& originalEflowLayerIntegrator);
  eflowLayerIntegrator&  operator=(const eflowLayerIntegrator& originalEflowLayerIntegrator);

  ~eflowLayerIntegrator();

  void measureNewClus(const xAOD::CaloCluster* clus, const eflowTrackCaloPoints& trackCalo);
  void measureNewClus(eflowTrackClusterLink* trackClusterLink);
  void measureNewClus(const std::vector<eflowRecCluster*>& efRecClusters, eflowRecTrack* track);
  void measureNewClus(const std::vector<xAOD::CaloCluster*>& clusVec, const eflowTrackCaloPoints& trackCalo);

  eflowFirstIntENUM getFirstIntLayer() const;

 private:
  void resetAllClustersIntegralForNewTrack(const eflowTrackCaloPoints& trackCalo);
  void addToAllClustersIntegral(const std::vector<double>& clusterIntegral);

  void measureCluster(eflowTrackClusterLink* trackClusterLink);
  void measureCluster(const xAOD::CaloCluster* clus, const eflowTrackCaloPoints& trackCalo);
  void measureCell(const CaloCell* cell, const eflowTrackCaloPoints& trackCalo);

  double m_rMax;

  /* Defines whether we are in HLLHC (Run4) setup or not (Runs 1,2 or 3) */
  bool m_isHLLHC;

  std::vector<double> m_allClustersIntegral;
  std::vector<double> m_singleClusterIntegral;
  
  eflowDepthCalculator m_caloModel;

  double m_densityConversion[eflowCalo::nRegions]{};

  std::vector<double> m_nUnitCellPerWindowOverCellEtaPhiArea;

  std::unique_ptr<eflowCellIntegrator<0> > m_integrator;
  std::unique_ptr<eflowCellIntegrator<1> > m_integratorLookup;
};

#endif
