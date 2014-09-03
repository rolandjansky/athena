/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOCLUSTERMOMENTSTORE_P1_H
#define CALOATHENAPOOL_CALOCLUSTERMOMENTSTORE_P1_H
#include <vector>

class CaloClusterMomentStore_p1 {
public:
  //Dummy default constructor
  //CaloClusterMomentStore() {}

  struct ClusterMoment {
    short key;
    float value;
  };
  std::vector<ClusterMoment> m_store;  
};

#endif


/*Necessary Top-Level Converter:
CaloClusterContainer
CaloShowerContainer
CaloCellLinkContainer
CaloTowerContainer (???)
*/


