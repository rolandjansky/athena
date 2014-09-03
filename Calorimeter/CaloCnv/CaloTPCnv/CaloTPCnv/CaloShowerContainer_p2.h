/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOSHOWERCONTAINER_P2_H
#define CALOATHENAPOOL_CALOSHOWERCONTAINER_P2_H

#include <vector>
#include "CaloTPCnv/CaloClusterMomentContainer_p1.h"
#include "CaloTPCnv/CaloSamplingDataContainer_p1.h"

class CaloShowerContainer_p2
{
public:
  CaloShowerContainer_p2() : m_nClusters(0) {}
  CaloClusterMomentContainer_p1 m_momentContainer;
  CaloSamplingDataContainer_p1 m_samplingDataContainer;
  unsigned int m_nClusters;
};

#endif


