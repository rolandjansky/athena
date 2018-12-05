/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1EVENTTPCNV_RODHeaderCOLLECTION_P1_H
#define TRIGT1EVENTTPCNV_RODHeaderCOLLECTION_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class RODHeaderCollection_p1 : public std::vector<TPObjRef>
{
 public:
 
  RODHeaderCollection_p1() {}

};

#endif
