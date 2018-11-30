/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CPMHITSCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CPMHITSCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CPMHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CPMHits_p1.h"

class CPMHitsCollection_tlp1
{
 public:
 
  CPMHitsCollection_tlp1() {};
  friend class CPMHitsCollectionCnv_tlp1;

 private:

  std::vector<CPMHitsCollection_p1>  m_CPMHitsCollection ;
  std::vector<CPMHits_p1>            m_CPMHits ;

};

#endif
