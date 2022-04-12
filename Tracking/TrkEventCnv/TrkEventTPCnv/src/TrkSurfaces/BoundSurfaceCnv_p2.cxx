/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   BoundSurfaceCnv_p2.cxx
//
// ----------------------------------------------------------------------------

#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkSurfaces/NoBounds.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p2.h"
#include <typeinfo>

template< class SURFACE, class BOUNDS_CNV >
void
BoundSurfaceCnv_p2< SURFACE, BOUNDS_CNV >::
persToTrans( const Trk::BoundSurface_p2* /**persObj*/, SURFACE* /**transObj*/, MsgStream & /**log*/ )
{
  throw std::runtime_error("BoundSurfaceCnv_p2::persToTrans shouldn't be called any more!");
}

template< class SURFACE, class BOUNDS_CNV >
SURFACE*
BoundSurfaceCnv_p2< SURFACE, BOUNDS_CNV >::createTransient( const Trk::BoundSurface_p2 * persObj, MsgStream& log)
{
   SURFACE*  transObj = m_surfaceCnv.createTransient( persObj, log );
   typename BOUNDS_CNV::Trans_t* bounds = this->createTransFromPStore( &m_boundsCnv, persObj->m_bounds, log );
   transObj->m_bounds = Trk::SharedObject<typename BOUNDS_CNV::Trans_t>(bounds);
   return transObj;
}


template< class SURFACE, class BOUNDS_CNV >
void
BoundSurfaceCnv_p2< SURFACE, BOUNDS_CNV >::
transToPers( const SURFACE *transObj, Trk::BoundSurface_p2 *persObj, MsgStream &log )
{
   m_surfaceCnv.transToPers( transObj, persObj, log );
   if ((transObj->m_bounds).get() && !dynamic_cast<const Trk::NoBounds*>((transObj->m_bounds).get()))
       persObj->m_bounds = this->toPersistent( &m_boundsCnv, (transObj->m_bounds).get(), log );
}

// specialized for polymorphic surfaces
using TPCnvForSurfBnds = ITPConverterFor<Trk::SurfaceBounds>;

template< class SURFACE >
void
BoundSurfaceCnv_p2< SURFACE, TPCnvForSurfBnds >::
persToTrans( const Trk::BoundSurface_p2 * /**persObj*/, SURFACE * /**transObj*/, MsgStream & /**log*/ )
{
  throw std::runtime_error("BoundSurfaceCnv_p2::persToTrans shouldn't be called any more!");
  // This is because in the SurfaceCnv we might need to return an already existing surface (belong to a DetElement)
}

template < class SURFACE >
SURFACE* 
BoundSurfaceCnv_p2< SURFACE >::createTransient( const Trk::BoundSurface_p2 * persObj, MsgStream& log)
{
   SURFACE*  transObj = m_surfaceCnv.createTransient( persObj, log );

   Trk::SurfaceBounds* bounds = this->createTransFromPStore( (TPCnvForSurfBnds**)nullptr, persObj->m_bounds, log );
   transObj->m_bounds = Trk::SharedObject<const Trk::SurfaceBounds>(bounds);
   return transObj;
}


template< class SURFACE >
void
BoundSurfaceCnv_p2< SURFACE, TPCnvForSurfBnds >::
transToPers( const SURFACE *transObj, Trk::BoundSurface_p2 *persObj, MsgStream &log )
{
   m_surfaceCnv.transToPers( transObj, persObj, log );
   if ((transObj->m_bounds).get() && !dynamic_cast<const Trk::NoBounds*>((transObj->m_bounds).get()))
       persObj->m_bounds = this->toPersistent( (TPCnvForSurfBnds**)nullptr, (transObj->m_bounds).get(), log );       
}


template class BoundSurfaceCnv_p2< Trk::DiscSurface, DiscBoundsCnv_p1 >;
template class BoundSurfaceCnv_p2< Trk::CylinderSurface, CylinderBoundsCnv_p1 >;
template class BoundSurfaceCnv_p2< Trk::StraightLineSurface, CylinderBoundsCnv_p1 >;
template class BoundSurfaceCnv_p2< Trk::SaggedLineSurface, CylinderBoundsCnv_p1 >;
template class BoundSurfaceCnv_p2< Trk::PlaneSurface >;
template class BoundSurfaceCnv_p2< Trk::ConeSurface,  ConeBoundsCnv_p1>;
