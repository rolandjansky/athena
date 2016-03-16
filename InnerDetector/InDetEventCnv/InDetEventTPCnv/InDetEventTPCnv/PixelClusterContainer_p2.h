/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINER_P2_H
#define PIXELCLUSTERCONTAINER_P2_H

/*

Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p2.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"

namespace InDet{
class PixelClusterContainer_p2   
{
 public:
/// Default constructor
  PixelClusterContainer_p2 ();
  // private:
  std::vector<InDet::InDetPRD_Collection_p2>  m_collections;
  std::vector<InDet::PixelCluster_p2>      m_rawdata;
};


// inlines

inline
PixelClusterContainer_p2::PixelClusterContainer_p2 () {}
}

#endif
