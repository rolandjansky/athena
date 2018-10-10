/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_JETELEMENTCOLLECTION_TLP1_H
#define TRIGT1EVENTTPCNV_JETELEMENTCOLLECTION_TLP1_H

#include "TrigT1EventTPCnv/JetElementCollection_p1.h"
#include "TrigT1EventTPCnv/JetElement_p1.h"

class JetElementCollection_tlp1
{
 public:
 
  JetElementCollection_tlp1() {};
  friend class JetElementCollectionCnv_tlp1;

 private:

  std::vector<JetElementCollection_p1>  m_JetElementCollection ;
  std::vector<JetElement_p1>            m_JetElements ;

};

#endif
