/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetLowBetaContainer_P1_H
#define InDetLowBetaContainer_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class InDetLowBetaContainer_p1 : public std::vector<TPObjRef> {
 public:
  InDetLowBetaContainer_p1()  { }
};  

#endif
