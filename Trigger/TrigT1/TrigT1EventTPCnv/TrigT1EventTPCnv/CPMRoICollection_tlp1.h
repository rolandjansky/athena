/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CPMRoICOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CPMRoICOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CPMRoICollection_p1.h"
#include "TrigT1EventTPCnv/CPMRoI_p1.h"

class CPMRoICollection_tlp1
{
 public:
 
  CPMRoICollection_tlp1() {};
  friend class CPMRoICollectionCnv_tlp1;

 private:

  std::vector<CPMRoICollection_p1>  m_CPMRoICollection ;
  std::vector<CPMRoI_p1>            m_CPMRoI ;

};

#endif
