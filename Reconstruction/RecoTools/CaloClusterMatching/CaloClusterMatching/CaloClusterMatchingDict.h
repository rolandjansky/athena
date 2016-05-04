/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METRECONSTRUCTIONDICT_H
#define METRECONSTRUCTIONDICT_H

#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

#include "CaloClusterMatching/ICaloClusterMatching.h"
#include "src/ICaloClusterMatching.h"
#include "src/CaloClusterMatchLinkAlg.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_CLUSTERMATCHING{
    std::pair<const xAOD::CaloCluster*,float> pair1;
    std::vector<std::pair<const xAOD::CaloCluster*,float> > vector1;
    ClusterSortMethod enum1;
  }
}

#endif //METRECONSTRUCTIONDICT_H
