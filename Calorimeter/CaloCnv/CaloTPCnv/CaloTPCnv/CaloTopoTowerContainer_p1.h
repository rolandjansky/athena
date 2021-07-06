/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOTOPOTOWERCONTAINER_P1_H
#define CALOATHENAPOOL_CALOTOPOTOWERCONTAINER_P1_H

#include "CaloTPCnv/CaloTowerSeg_p1.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include <vector>

class CaloTopoTowerContainer_p1 
{
 public:
  CaloTowerSeg_p1 m_towerSeg;
  std::vector<unsigned int> m_caloRegions;

  DataLink_p1  cells;
  DataLink_p1  clusters;
  DataLink_p1  towers;

  double m_minimumCellEnergy;
  double m_minimumClusterEnergy;
  bool   m_useCellWeights;
  bool   m_useNoiseTool = false;  // now unused
  bool   m_usePileUpNoise = false; // now unused
  float  m_noiseSigma;
  float  m_cellESignificanceThreshold;
  bool   m_caloSelection;
};

#endif
