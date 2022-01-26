/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   BoundSurfaceCnv_p1.cxx
//
// ----------------------------------------------------------------------------

#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkSurfaces/NoBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p1.h"
#include <typeinfo>

template< class SURFACE, class BOUNDS_CNV >
void
BoundSurfaceCnv_p1< SURFACE, BOUNDS_CNV >::
persToTrans( const Trk::BoundSurface_p1 *persObj, SURFACE *transObj, MsgStream &log )
{
   m_surfaceCnv.persToTrans( persObj, transObj, log );
   Trk::SurfaceBounds* bounds = this->createTransFromPStore( &m_boundsCnv, persObj->m_bounds, log );
   transObj->m_bounds = Trk::SharedObject<const Trk::SurfaceBounds>(bounds);
}

// Specialization for CylinderSurface (most surfaces have m_bound == SurfaceBounds*, but Cyl Surf is limited to having Cylinder Bounds.)
template< >
void
BoundSurfaceCnv_p1< Trk::CylinderSurface, CylinderBoundsCnv_p1 >::
persToTrans( const Trk::BoundSurface_p1 *persObj, Trk::CylinderSurface *transObj, MsgStream &log )
{
   m_surfaceCnv.persToTrans( persObj, transObj, log );
   Trk::CylinderBounds* bounds = this->createTransFromPStore( &m_boundsCnv, persObj->m_bounds, log );
   transObj->m_bounds = Trk::SharedObject<const Trk::CylinderBounds>(bounds);
}

// Specialization for StraightLineSurface (most surfaces have m_bound == SurfaceBounds*, but StraightLineSurface is limited to having Cylinder Bounds.)
template< >
void
BoundSurfaceCnv_p1< Trk::StraightLineSurface, CylinderBoundsCnv_p1 >::
persToTrans( const Trk::BoundSurface_p1 *persObj, Trk::StraightLineSurface *transObj, MsgStream &log )
{
   m_surfaceCnv.persToTrans( persObj, transObj, log );
   Trk::CylinderBounds* bounds = this->createTransFromPStore( &m_boundsCnv, persObj->m_bounds, log );
   transObj->m_bounds = Trk::SharedObject<const Trk::CylinderBounds>(bounds);
}



template< class SURFACE, class BOUNDS_CNV >
void
BoundSurfaceCnv_p1< SURFACE, BOUNDS_CNV >::
transToPers( const SURFACE *transObj, Trk::BoundSurface_p1 *persObj, MsgStream &log )
{
   m_surfaceCnv.transToPers( transObj, persObj, log );
   if ((transObj->m_bounds).get() && !dynamic_cast<const Trk::NoBounds*>((transObj->m_bounds).get()))
       // persObj->m_bounds = this->toPersistent( &m_boundsCnv, (transObj->m_bounds).get(), log );
     log<<MSG::WARNING<<" No longer writing out bounds... "<<endmsg;
}



// specialized for polymorphic surfaces
typedef  ITPConverterFor<Trk::SurfaceBounds>   TPCnvForSurfBnds;

template< class SURFACE >
void
BoundSurfaceCnv_p1< SURFACE, TPCnvForSurfBnds >::
persToTrans( const Trk::BoundSurface_p1 *persObj, SURFACE *transObj, MsgStream &log )
{
   m_surfaceCnv.persToTrans( persObj, transObj, log );
   Trk::SurfaceBounds* bounds = this->createTransFromPStore( (TPCnvForSurfBnds**)nullptr, persObj->m_bounds, log );
   transObj->m_bounds = Trk::SharedObject<const Trk::SurfaceBounds>(bounds);
}


template< class SURFACE >
void
BoundSurfaceCnv_p1< SURFACE, TPCnvForSurfBnds >::
transToPers( const SURFACE *transObj, Trk::BoundSurface_p1 *persObj, MsgStream &log )
{
    //std::cout<<"transToPers for "<<typeid(*transObj).name()<<std::endl;
   m_surfaceCnv.transToPers( transObj, persObj, log );
   if ((transObj->m_bounds).get() && !dynamic_cast<const Trk::NoBounds*>((transObj->m_bounds).get()))
      // std::cout<<"Trying to convert "<<typeid( (transObj->m_bounds).getRef() ).name()<<" using "<<typeid( TPCnvForSurfBnds ).name()<<std::endl;   
       // persObj->m_bounds = this->toPersistent( (TPCnvForSurfBnds**)0, (transObj->m_bounds).get(), log );
       log<<MSG::WARNING<<" No longer writing out bounds... "<<endmsg;
       
}


template class BoundSurfaceCnv_p1< Trk::DiscSurface, DiscBoundsCnv_p1 >;
template class BoundSurfaceCnv_p1< Trk::CylinderSurface, CylinderBoundsCnv_p1 >;
template class BoundSurfaceCnv_p1< Trk::StraightLineSurface, CylinderBoundsCnv_p1 >;
template class BoundSurfaceCnv_p1< Trk::PlaneSurface >;
