/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CMMJetHitsCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CMMJetHitsCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CMMJetHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMJetHits_p1.h"

class CMMJetHitsCollection_tlp1
{
 public:
 
  CMMJetHitsCollection_tlp1() {};
  friend class CMMJetHitsCollectionCnv_tlp1;

 private:

  std::vector<CMMJetHitsCollection_p1>  m_CMMJetHitsCollection ;
  std::vector<CMMJetHits_p1>            m_CMMJetHits ;

};

#endif
