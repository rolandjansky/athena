/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CMMEtSumsCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CMMEtSumsCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CMMEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMEtSums_p1.h"

class CMMEtSumsCollection_tlp1
{
 public:
 
  CMMEtSumsCollection_tlp1() {};
  friend class CMMEtSumsCollectionCnv_tlp1;

 private:

  std::vector<CMMEtSumsCollection_p1>  m_CMMEtSumsCollection ;
  std::vector<CMMEtSums_p1>            m_CMMEtSums ;

};

#endif
