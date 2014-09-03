/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOTOWERCONTAINER_P1_H
#define CALOATHENAPOOL_CALOTOWERCONTAINER_P1_H

#include "CaloTPCnv/CaloTowerSeg_p1.h"
#include <vector>

class CaloTowerContainer_p1 
{
 public:
  CaloTowerSeg_p1 m_towerSeg;
  std::vector<unsigned int> m_caloRegions;
};

#endif
