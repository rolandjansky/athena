/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOATHENAPOOL_CALOCLUSTERMOMENTCONTAINER_P1_H
#define CALOATHENAPOOL_CALOCLUSTERMOMENTCONTAINER_P1_H
#include <vector>

class CaloClusterMomentContainer_p1 {
public:
  CaloClusterMomentContainer_p1() : m_nMoments() {}
  
  struct ClusterMoment_p {
    short key;
    float value;
  };
  std::vector<ClusterMoment_p> m_store;  
  unsigned short m_nMoments; 
  typedef std::vector<ClusterMoment_p>::const_iterator const_iterator;

};

#endif



