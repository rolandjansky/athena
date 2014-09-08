/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOUND_SURFACE_CNV_P1_H
#define BOUND_SURFACE_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   BoundSurfaceCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/CylinderSurface.h"

#include "TrkEventTPCnv/TrkSurfaces/BoundSurface_p1.h"

#include "SurfaceCnv_p1.h"
#include "CylinderBoundsCnv_p1.h"
#include "DiscBoundsCnv_p1.h"

class MsgStream;

template< class SURFACE, class BOUNDS_CNV = ITPConverterFor<Trk::SurfaceBounds> >
class BoundSurfaceCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::Surface, SURFACE, Trk::BoundSurface_p1>
{
public:
  BoundSurfaceCnv_p1() : m_boundsCnv(0) {}
  
  void persToTrans( const Trk::BoundSurface_p1 *, SURFACE *, MsgStream & );
  void transToPers( const SURFACE *, Trk::BoundSurface_p1 *, MsgStream & );

private:
  SurfaceCnv_p1 	m_surfaceCnv;  // Base class convertor
  BOUNDS_CNV 		*m_boundsCnv;
};




template< class SURFACE >
class BoundSurfaceCnv_p1< SURFACE, ITPConverterFor<Trk::SurfaceBounds> >
   : public T_AthenaPoolTPPolyCnvBase< Trk::Surface, SURFACE, Trk::BoundSurface_p1>
{
public:
  void persToTrans( const Trk::BoundSurface_p1 *, SURFACE *, MsgStream & );
  void transToPers( const SURFACE *, Trk::BoundSurface_p1 *, MsgStream & );

private:
  SurfaceCnv_p1 	m_surfaceCnv;  
};

class DiscSurfaceCnv_p1
   : public BoundSurfaceCnv_p1< Trk::DiscSurface, DiscBoundsCnv_p1 > {};

class CylinderSurfaceCnv_p1
   : public BoundSurfaceCnv_p1< Trk::CylinderSurface, CylinderBoundsCnv_p1 > {};

class StraightLineSurfaceCnv_p1
   : public BoundSurfaceCnv_p1< Trk::StraightLineSurface, CylinderBoundsCnv_p1 > {};

class PlaneSurfaceCnv_p1
   : public BoundSurfaceCnv_p1< Trk::PlaneSurface > {};

#endif 




