/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CPMTobRoICOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CPMTobRoICOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CPMTobRoICollection_p1.h"
#include "TrigT1EventTPCnv/CPMTobRoI_p1.h"

class CPMTobRoICollection_tlp1
{
 public:
 
  CPMTobRoICollection_tlp1() {};
  friend class CPMTobRoICollectionCnv_tlp1;

 private:

  std::vector<CPMTobRoICollection_p1>  m_CPMTobRoICollection ;
  std::vector<CPMTobRoI_p1>            m_CPMTobRoI ;

};

#endif
