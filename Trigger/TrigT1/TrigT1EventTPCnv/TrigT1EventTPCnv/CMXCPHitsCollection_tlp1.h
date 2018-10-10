/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CMXCPHitsCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CMXCPHitsCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CMXCPHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXCPHits_p1.h"

class CMXCPHitsCollection_tlp1
{
 public:
 
  CMXCPHitsCollection_tlp1() {};
  friend class CMXCPHitsCollectionCnv_tlp1;

 private:

  std::vector<CMXCPHitsCollection_p1>  m_CMXCPHitsCollection ;
  std::vector<CMXCPHits_p1>            m_CMXCPHits ;

};

#endif
