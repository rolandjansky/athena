/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   AtaSurfaceCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkParameters/TrackParameters.h"


#include "TrkEventTPCnv/TrkParameters/AtaSurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkParameters/AtaSurfaceCnv_p1.icc"


template class AtaSurfaceCnv_p1< DiscSurfaceCnv_p1, Trk::AtaDisc >;
template class AtaSurfaceCnv_p1< CylinderSurfaceCnv_p1, Trk::AtaCylinder >;
template class AtaSurfaceCnv_p1< PlaneSurfaceCnv_p1, Trk::AtaPlane >;
template class AtaSurfaceCnv_p1< StraightLineSurfaceCnv_p1, Trk::AtaStraightLine >;
template class AtaSurfaceCnv_p1< PlaneSurfaceCnv_p1, Trk::CurvilinearParameters >;
