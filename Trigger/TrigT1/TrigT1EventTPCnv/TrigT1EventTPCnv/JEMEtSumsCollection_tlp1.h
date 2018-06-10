/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_JEMEtSumsCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_JEMEtSumsCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/JEMEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/JEMEtSums_p1.h"

class JEMEtSumsCollection_tlp1
{
 public:
 
  JEMEtSumsCollection_tlp1() {};
  friend class JEMEtSumsCollectionCnv_tlp1;

 private:

  std::vector<JEMEtSumsCollection_p1>  m_JEMEtSumsCollection ;
  std::vector<JEMEtSums_p1>            m_JEMEtSums ;

};

#endif
