/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      transObj->m_transform = new Amg::Transform3D();
      // const std::vector<float>& vec = persObj->m_transform;
      // Amg::Transform3D& transform = *(transObj->m_transform);
      // std::cout<<"SurfaceCnv_p1::persToTrans - making Amg::Transform3D from vector:"<<std::endl;
      // for (auto v : persObj->m_transform)
      //     std::cout<< v<<" ";
      // std::cout<<std::endl;
          
      // EigenHelpers::vectorToEigenTransform3D( persObj->m_transform, *transObj->m_transform );
      (*transObj->m_transform)(0,0)=persObj->m_transform[0];
      (*transObj->m_transform)(0,1)=persObj->m_transform[1];
      (*transObj->m_transform)(0,2)=persObj->m_transform[2];
      (*transObj->m_transform)(0,3)=persObj->m_transform[3];
      (*transObj->m_transform)(1,0)=persObj->m_transform[4];
      (*transObj->m_transform)(1,1)=persObj->m_transform[5];
      (*transObj->m_transform)(1,2)=persObj->m_transform[6];
      (*transObj->m_transform)(1,3)=persObj->m_transform[7];
      (*transObj->m_transform)(2,0)=persObj->m_transform[8];
      (*transObj->m_transform)(2,1)=persObj->m_transform[9];
      (*transObj->m_transform)(2,2)=persObj->m_transform[10];
      (*transObj->m_transform)(2,3)=persObj->m_transform[11];
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

