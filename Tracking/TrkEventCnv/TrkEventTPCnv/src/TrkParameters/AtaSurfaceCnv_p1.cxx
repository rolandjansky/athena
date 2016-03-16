/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   AtaSurfaceCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkParameters/TrackParameters.h"


#include "TrkEventTPCnv/TrkParameters/AtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/AtaSurfaceCnv_p1.icc"


namespace {
   AtaDiscCnv_p1	inst1;
   AtaCylinderCnv_p1	inst2;
   AtaPlaneCnv_p1	inst3;
   AtaStraightLineCnv_p1 isnt4;
   CurvilinearParametersCnv_p1 isnt5; 
}

