/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOATHENAPOOL_CALOSHOWERCONTAINER_P1_H
#define CALOATHENAPOOL_CALOSHOWERCONTAINER_P1_H

#include <vector>
#include "CaloTPCnv/CaloClusterMomentStore_p1.h"
#include "CaloTPCnv/CaloSamplingData_p1.h"

class CaloShowerContainer_p1
{
public:
  //friend class CaloShowerContainerCnv_p1;
  /** constructor */
  CaloShowerContainer_p1() { };

  struct CaloShower {
    CaloClusterMomentStore_p1 m_momentStore;
    CaloSamplingData_p1 m_samplingStore;
  };

  std::vector<CaloShower> m_container;
  typedef std::vector<CaloShower>::const_iterator const_iterator;
};

#endif


