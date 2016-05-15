/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPRD_CONTAINER_P1_H
#define MUONPRD_CONTAINER_P1_H

#include <vector>
#include <string>
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Collection_p1.h"

namespace Muon {
class MuonPRD_Container_p1   
{
 public:
/// Default constructor
  MuonPRD_Container_p1 ();
  //private:
  std::vector<Muon::MuonPRD_Collection_p1>  m_collections;
  std::vector<TPObjRef>                     m_PRD;
};

} 

// inlines
 
inline Muon::MuonPRD_Container_p1::MuonPRD_Container_p1() {}
 
#endif
