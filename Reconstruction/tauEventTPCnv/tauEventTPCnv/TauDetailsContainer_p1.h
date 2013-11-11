/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauDetailsContainer_p1.cxx
// package:     Reconstruction/tauEventTPCnv
// authors:     Lukasz Janyst
// date:        2007-07-06
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAU_DETAILS_CONTAINER_P1_H
#define tauEventTPCnv_TAU_DETAILS_CONTAINER_P1_H

#include <vector>
#include "AthenaPoolUtilities/TPObjRef.h"

class TauDetailsContainer_p1: public std::vector<TPObjRef>
{
};

#endif // tauEventTPCnv_TAU_DETAILS_CONTAINER_P1_H
