/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CMXJetHitsCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CMXJetHitsCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CMXJetHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXJetHits_p1.h"

class CMXJetHitsCollection_tlp1
{
 public:
 
  CMXJetHitsCollection_tlp1() {};
  friend class CMXJetHitsCollectionCnv_tlp1;

 private:

  std::vector<CMXJetHitsCollection_p1>  m_CMXJetHitsCollection ;
  std::vector<CMXJetHits_p1>            m_CMXJetHits ;

};

#endif
