/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CMMCPHitsCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CMMCPHitsCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CMMCPHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMCPHits_p1.h"

class CMMCPHitsCollection_tlp1
{
 public:
 
  CMMCPHitsCollection_tlp1() {};
  friend class CMMCPHitsCollectionCnv_tlp1;

 private:

  std::vector<CMMCPHitsCollection_p1>  m_CMMCPHitsCollection ;
  std::vector<CMMCPHits_p1>            m_CMMCPHits ;

};

#endif
