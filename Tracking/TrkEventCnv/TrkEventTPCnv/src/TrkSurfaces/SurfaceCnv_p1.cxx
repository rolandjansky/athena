/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SurfaceCnv_p1.cxx
//
// ----------------------------------------------------------------------------

#include "TrkSurfaces/Surface.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

//-----------------------------------------------------------------------------
// Persistent to Transient
//-----------------------------------------------------------------------------
void SurfaceCnv_p1::persToTrans( const Trk::Surface_p1 *persObj, Trk::Surface *transObj, MsgStream &) {
   transObj->m_associatedDetElementId = Identifier(Identifier32(persObj->m_associatedDetElementId));
   if( persObj->m_transform.size() ) {

      Amg::Transform3D trans{};
      trans(0,0)=persObj->m_transform[0];
      trans(0,1)=persObj->m_transform[1];
      trans(0,2)=persObj->m_transform[2];
      trans(0,3)=persObj->m_transform[3];
      trans(1,0)=persObj->m_transform[4];
      trans(1,1)=persObj->m_transform[5];
      trans(1,2)=persObj->m_transform[6];
      trans(1,3)=persObj->m_transform[7];
      trans(2,0)=persObj->m_transform[8];
      trans(2,1)=persObj->m_transform[9];
      trans(2,2)=persObj->m_transform[10];
      trans(2,3)=persObj->m_transform[11];
      transObj->setTransform(trans);
   }
}

//-----------------------------------------------------------------------------
// Transient to Persistent
//-----------------------------------------------------------------------------
void SurfaceCnv_p1::transToPers( const Trk::Surface    * ,
                                       Trk::Surface_p1 * ,
                                       MsgStream &)
{
  throw std::runtime_error("SurfaceCnv_p1::transToPers is deprecated!");
  // persObj->m_associatedDetElementId = transObj->m_associatedDetElementId.get_compact();
  // if( transObj->m_transform ) {
  //    EigenHelpers::eigenTransform3DToVector( *transObj->m_transform,
  //                                          persObj->m_transform );
  // }
}

