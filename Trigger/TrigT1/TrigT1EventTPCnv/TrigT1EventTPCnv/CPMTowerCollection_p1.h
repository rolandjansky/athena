/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_CPMTowerCOLLECTION_P1_H
#define TRIGT1EVENTTPCNV_CPMTowerCOLLECTION_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class CPMTowerCollection_p1 : public std::vector<TPObjRef>
{
 public:
 
  CPMTowerCollection_p1() {}

};

#endif
