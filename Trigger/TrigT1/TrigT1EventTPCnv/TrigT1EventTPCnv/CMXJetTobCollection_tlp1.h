/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CMXJetTobCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_CMXJetTobCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/CMXJetTobCollection_p1.h"
#include "TrigT1EventTPCnv/CMXJetTob_p1.h"

class CMXJetTobCollection_tlp1
{
 public:
 
  CMXJetTobCollection_tlp1() {};
  friend class CMXJetTobCollectionCnv_tlp1;

 private:

  std::vector<CMXJetTobCollection_p1>  m_CMXJetTobCollection ;
  std::vector<CMXJetTob_p1>            m_CMXJetTob ;

};

#endif
