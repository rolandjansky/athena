/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CMXCPTobCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CMXCPTobCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CMXCPTobCollection_p1.h"
#include "TrigT1EventTPCnv/CMXCPTob_p1.h"

class CMXCPTobCollection_tlp1
{
 public:
 
  CMXCPTobCollection_tlp1() {};
  friend class CMXCPTobCollectionCnv_tlp1;

 private:

  std::vector<CMXCPTobCollection_p1>  m_CMXCPTobCollection ;
  std::vector<CMXCPTob_p1>            m_CMXCPTob ;

};

#endif
