/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   NeutralAtaSurfaceCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"

#include "TrkEventTPCnv/TrkNeutralParameters/NeutralAtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkNeutralParameters/NeutralAtaSurfaceCnv_p1.icc"


namespace {
   NeutralAtaDiscCnv_p1	inst1;
   NeutralAtaCylinderCnv_p1	inst2;
   NeutralAtaPlaneCnv_p1	inst3;
   NeutralAtaStraightLineCnv_p1 isnt4;
}

