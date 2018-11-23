/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_TRIGGERTOWERCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_TRIGGERTOWERCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/TriggerTowerCollection_p1.h"
#include "TrigT1EventTPCnv/TriggerTower_p1.h"

class TriggerTowerCollection_tlp1
{
 public:
 
  TriggerTowerCollection_tlp1() {};
  friend class TriggerTowerCollectionCnv_tlp1;

 private:

  std::vector<TriggerTowerCollection_p1>  m_TriggerTowerCollection ;
  std::vector<TriggerTower_p1>            m_TriggerTowers ;

};

#endif
