/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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


template class NeutralAtaSurfaceCnv_p1< DiscSurfaceCnv_p1, Trk::NeutralAtaDisc >;
template class NeutralAtaSurfaceCnv_p1< CylinderSurfaceCnv_p1, Trk::NeutralAtaCylinder >;
template class NeutralAtaSurfaceCnv_p1< PlaneSurfaceCnv_p1, Trk::NeutralAtaPlane >;
template class NeutralAtaSurfaceCnv_p1< StraightLineSurfaceCnv_p1, Trk::NeutralAtaStraightLine >;
