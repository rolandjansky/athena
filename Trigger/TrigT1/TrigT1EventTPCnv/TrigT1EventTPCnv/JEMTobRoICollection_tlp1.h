/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_JEMTobRoICOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_JEMTobRoICOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/JEMTobRoICollection_p1.h"
#include "TrigT1EventTPCnv/JEMTobRoI_p1.h"

class JEMTobRoICollection_tlp1
{
 public:
 
  JEMTobRoICollection_tlp1() {};
  friend class JEMTobRoICollectionCnv_tlp1;

 private:

  std::vector<JEMTobRoICollection_p1>  m_JEMTobRoICollection ;
  std::vector<JEMTobRoI_p1>            m_JEMTobRoI ;

};

#endif
