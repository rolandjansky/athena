/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLANARCLUSTERCONTAINER_P2_H
#define PLANARCLUSTERCONTAINER_P2_H

#include <vector>
#include <string>
#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarCluster_p2.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"

namespace iFatras{
 class PlanarClusterContainer_p2   
{
 public:
/// Default constructor
  PlanarClusterContainer_p2 ();
  // private:
  std::vector<InDet::InDetPRD_Collection_p2>  m_collections;
  std::vector<iFatras::PlanarCluster_p2>      m_rawdata;
};


inline
PlanarClusterContainer_p2::PlanarClusterContainer_p2 () {}
}

#endif // PLANARCLUSTERCONTAINER_P2_H
