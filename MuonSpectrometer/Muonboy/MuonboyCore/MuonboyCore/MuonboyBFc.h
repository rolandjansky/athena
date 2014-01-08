/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyBFc_H
#define MuonboyBFc_H


#include "MuonboyCore/MuonboyCoreMisc.h"

extern "C" void muonboybfield_(float* XYZ,float* BXYZ);

extern "C" void muonboybfieldd_(float* XYZ,float* BXYZ);

extern "C" void muonboybfieldo_(float* XYZ,float* BXYZ);

extern "C" void muonboybfielddo_(float* XYZ,float* BXYZ);

#endif
