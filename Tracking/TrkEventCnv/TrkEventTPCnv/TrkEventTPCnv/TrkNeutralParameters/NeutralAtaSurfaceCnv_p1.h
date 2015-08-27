/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NeutralAtaSurfaceCnv_p1_H
#define NeutralAtaSurfaceCnv_p1_H

//-----------------------------------------------------------------------------
//
// file:   NeutralAtaSurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParameters/TrackParameters.h"

#include "NeutralParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p1.h"

#include "TrkEventTPCnv/TrkParameters/AtaSurface_p1.h"
#include "TrkEventTPCnv/TrkNeutralParameters/NeutralParametersCnv_p1.h"

#include "TrkNeutralParameters/NeutralParameters.h"


class MsgStream;

template< class SURFACE_CNV, class ATA_SURFACE >
class NeutralAtaSurfaceCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::NeutralParameters, ATA_SURFACE, Trk::AtaSurface_p1>
{
public:
  NeutralAtaSurfaceCnv_p1(): m_paramCnv(0), m_surfaceCnv(0) {}
  
  void persToTrans( const Trk::AtaSurface_p1 *, ATA_SURFACE *, MsgStream & );
  void transToPers( const ATA_SURFACE *, Trk::AtaSurface_p1 *, MsgStream & );

private:
  NeutralParametersCnv_p1 *m_paramCnv; // base class
  SURFACE_CNV 		*m_surfaceCnv; // always same type (hence no ITPConverterFor)
};


class NeutralAtaDiscCnv_p1 : public  NeutralAtaSurfaceCnv_p1< DiscSurfaceCnv_p1, Trk::NeutralAtaDisc > {};
class NeutralAtaCylinderCnv_p1 : public  NeutralAtaSurfaceCnv_p1< CylinderSurfaceCnv_p1, Trk::NeutralAtaCylinder > {};
class NeutralAtaPlaneCnv_p1 : public  NeutralAtaSurfaceCnv_p1< PlaneSurfaceCnv_p1, Trk::NeutralAtaPlane > {};
class NeutralAtaStraightLineCnv_p1 : public  NeutralAtaSurfaceCnv_p1< StraightLineSurfaceCnv_p1, Trk::NeutralAtaStraightLine > {};

#endif












