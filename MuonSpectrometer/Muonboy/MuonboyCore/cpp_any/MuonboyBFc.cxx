/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyBFAccess.h"

#include "MboyCoreEvt/MuonboyBFAbstract.h"

extern "C"
void muonboybfield_(float* XYZ,float* BXYZ){

  MuonboyBFAccess::field_tesla_cm(XYZ,BXYZ);

}

extern "C"
void muonboybfieldd_(float* XYZ,float* BXYZ){

  MuonboyBFAccess::fieldGradient_tesla_cm(XYZ,BXYZ);

}

extern "C"
void muonboybfieldo_(float* XYZ,float* BXYZ){

  MuonboyBFAccess::field(XYZ,BXYZ);

}

extern "C"
void muonboybfielddo_(float* XYZ,float* BXYZ){

  MuonboyBFAccess::fieldd(XYZ,BXYZ);

}

