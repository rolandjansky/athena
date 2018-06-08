/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CMXEtSumsCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CMXEtSumsCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CMXEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXEtSums_p1.h"

class CMXEtSumsCollection_tlp1
{
 public:
 
  CMXEtSumsCollection_tlp1() {};
  friend class CMXEtSumsCollectionCnv_tlp1;

 private:

  std::vector<CMXEtSumsCollection_p1>  m_CMXEtSumsCollection ;
  std::vector<CMXEtSums_p1>            m_CMXEtSums ;

};

#endif
