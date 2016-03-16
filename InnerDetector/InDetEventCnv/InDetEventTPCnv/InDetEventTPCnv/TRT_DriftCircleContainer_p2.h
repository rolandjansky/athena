/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINER_P2_H
#define TRT_DRIFTCIRCLECONTAINER_P2_H

/*

Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircle_p2.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"

namespace InDet{
class TRT_DriftCircleContainer_p2   
{
 public:
/// Default constructor
  TRT_DriftCircleContainer_p2 ();
  //private:
  std::vector<InDet::InDetPRD_Collection_p2>  m_collections;
  std::vector<InDet::TRT_DriftCircle_p2>      m_rawdata;
// The delta identifiers of the PRD:
  std::vector<unsigned short>                 m_prdDeltaId;
};


// inlines

inline
TRT_DriftCircleContainer_p2::TRT_DriftCircleContainer_p2 () {}
}

#endif
