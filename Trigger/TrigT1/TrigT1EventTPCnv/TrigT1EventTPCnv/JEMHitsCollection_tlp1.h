/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_JEMHitsCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_JEMHitsCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/JEMHitsCollection_p1.h"
#include "TrigT1EventTPCnv/JEMHits_p1.h"

class JEMHitsCollection_tlp1
{
 public:
 
  JEMHitsCollection_tlp1() {};
  friend class JEMHitsCollectionCnv_tlp1;

 private:

  std::vector<JEMHitsCollection_p1>  m_JEMHitsCollection ;
  std::vector<JEMHits_p1>            m_JEMHits ;

};

#endif
