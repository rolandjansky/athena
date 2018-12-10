/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CPMTowerCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CPMTowerCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CPMTowerCollection_p1.h"
#include "TrigT1EventTPCnv/CPMTower_p1.h"

class CPMTowerCollection_tlp1
{
 public:
 
  CPMTowerCollection_tlp1() {};
  friend class CPMTowerCollectionCnv_tlp1;

 private:

  std::vector<CPMTowerCollection_p1>  m_CPMTowerCollection ;
  std::vector<CPMTower_p1>            m_CPMTowers ;

};

#endif
