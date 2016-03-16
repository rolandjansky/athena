/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPRD_CONTAINER_P1_H
#define INDETPRD_CONTAINER_P1_H 1

#include <vector>
#include <string>
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p1.h"

namespace InDet {
class InDetPRD_Container_p1   
{
 public:
/// Default constructor
  InDetPRD_Container_p1 ();
  //private:
  std::vector<InDet::InDetPRD_Collection_p1>  m_collections;
  std::vector<TPObjRef>             m_PRD;
};

} 


// inlines
 
inline InDet::InDetPRD_Container_p1::InDetPRD_Container_p1() {}
 
#endif
