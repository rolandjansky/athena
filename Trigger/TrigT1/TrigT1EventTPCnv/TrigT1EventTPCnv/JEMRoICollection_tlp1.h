/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_JEMRoICOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_JEMRoICOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/JEMRoICollection_p1.h"
#include "TrigT1EventTPCnv/JEMRoI_p1.h"

class JEMRoICollection_tlp1
{
 public:
 
  JEMRoICollection_tlp1() {};
  friend class JEMRoICollectionCnv_tlp1;

 private:

  std::vector<JEMRoICollection_p1>  m_JEMRoICollection ;
  std::vector<JEMRoI_p1>            m_JEMRoI ;

};

#endif
