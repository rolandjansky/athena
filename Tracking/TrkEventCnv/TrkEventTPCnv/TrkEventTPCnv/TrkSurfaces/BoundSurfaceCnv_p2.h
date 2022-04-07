/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOUND_SURFACE_CNV_P2_H
#define BOUND_SURFACE_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   BoundSurfaceCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkDistortedSurfaces/SaggedLineSurface.h"

#include "TrkEventTPCnv/TrkSurfaces/BoundSurface_p2.h"

#include "SurfaceCnv_p2.h"
#include "CylinderBoundsCnv_p1.h"
#include "DiscBoundsCnv_p1.h"
#include "ConeBoundsCnv_p1.h"

class MsgStream;

template< class SURFACE, class BOUNDS_CNV = ITPConverterFor<Trk::SurfaceBounds> >
class BoundSurfaceCnv_p2
   : public T_AthenaPoolTPPolyCnvBase< Trk::Surface, SURFACE, Trk::BoundSurface_p2>
{
public:
  BoundSurfaceCnv_p2() : m_boundsCnv(0) {}
  
  void persToTrans( const Trk::BoundSurface_p2 *, SURFACE *, MsgStream & );
  void transToPers( const SURFACE *, Trk::BoundSurface_p2 *, MsgStream & );
  SURFACE* createTransient( const Trk::BoundSurface_p2 * persObj, MsgStream& log);

private:
  SurfaceCnv_p2 < SURFACE > m_surfaceCnv;  // Base class convertor
  BOUNDS_CNV 		*m_boundsCnv;
};




template< class SURFACE >
class BoundSurfaceCnv_p2< SURFACE, ITPConverterFor<Trk::SurfaceBounds> >
   : public T_AthenaPoolTPPolyCnvBase< Trk::Surface, SURFACE, Trk::BoundSurface_p2>
{
public:
  void persToTrans( const Trk::BoundSurface_p2 *, SURFACE *, MsgStream & );
  void transToPers( const SURFACE *, Trk::BoundSurface_p2 *, MsgStream & );
  SURFACE* createTransient( const Trk::BoundSurface_p2 * persObj, MsgStream& log);

private:
  SurfaceCnv_p2 <SURFACE> 	m_surfaceCnv;  
};

class BoundDiscSurfaceCnv_p2
   : public BoundSurfaceCnv_p2< Trk::DiscSurface, DiscBoundsCnv_p1 > {};

class BoundCylinderSurfaceCnv_p2
   : public BoundSurfaceCnv_p2< Trk::CylinderSurface, CylinderBoundsCnv_p1 > {};

class BoundStraightLineSurfaceCnv_p2
   : public BoundSurfaceCnv_p2< Trk::StraightLineSurface, CylinderBoundsCnv_p1 > {};

class BoundPlaneSurfaceCnv_p2
   : public BoundSurfaceCnv_p2< Trk::PlaneSurface > {};

class BoundConeSurfaceCnv_p2
   : public BoundSurfaceCnv_p2< Trk::ConeSurface, ConeBoundsCnv_p1 > {};

class BoundSaggedLineSurfaceCnv_p2
   : public BoundSurfaceCnv_p2< Trk::SaggedLineSurface, CylinderBoundsCnv_p1 > {};
   

// PerigeeSurface is boundless.


#endif 




