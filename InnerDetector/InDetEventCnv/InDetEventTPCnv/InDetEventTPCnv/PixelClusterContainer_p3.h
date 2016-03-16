/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINER_P3_H
#define PIXELCLUSTERCONTAINER_P3_H

/*

Author: Attilio Andreazza

*/

#include <vector>
#include <string>
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p3.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"

namespace InDet{
class PixelClusterContainer_p3   
{
 public:
/// Default constructor
  PixelClusterContainer_p3 ();
  //private:
  std::vector<InDet::InDetPRD_Collection_p2>  m_collections;
  std::vector<InDet::PixelCluster_p3>      m_rawdata;
};


// inlines

inline
PixelClusterContainer_p3::PixelClusterContainer_p3 () {}
}

#endif
