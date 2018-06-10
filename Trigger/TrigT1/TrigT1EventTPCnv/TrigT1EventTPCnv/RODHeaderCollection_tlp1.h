/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_RODHeaderCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_RODHeaderCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/RODHeaderCollection_p1.h"
#include "TrigT1EventTPCnv/RODHeader_p1.h"

class RODHeaderCollection_tlp1
{
 public:
 
  RODHeaderCollection_tlp1() {};
  friend class RODHeaderCollectionCnv_tlp1;

 private:

  std::vector<RODHeaderCollection_p1>  m_RODHeaderCollection ;
  std::vector<RODHeader_p1>            m_RODHeaders ;

};

#endif
