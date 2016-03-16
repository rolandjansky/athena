/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrackCnv_p1.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void VertexOnTrackCnv_p1::persToTrans( const Trk :: VertexOnTrack_p1 *persObj,
                                                        Trk :: VertexOnTrack    *transObj, MsgStream &log )
{
  Trk::LocalParameters localParams;
  fillTransFromPStore( &m_localParamsCnv, persObj->m_localParams, &localParams, log );
  // fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &transObj->m_localErrMat, log );
  Trk::ErrorMatrix dummy;
  Amg::MatrixX localCovariance;
  fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, localCovariance, "RIO_OnTrackCnv_p2");
   
  Trk::SurfaceUniquePtrT<const Trk::PerigeeSurface> surf
    (createTransFromPStore( &m_surfaceCnv, persObj->m_associatedSurface, log ));
  *transObj = Trk::VertexOnTrack (localParams, localCovariance, std::move(surf));
}

void VertexOnTrackCnv_p1::transToPers( const Trk :: VertexOnTrack    * /**transObj*/,
                                                        Trk :: VertexOnTrack_p1 * /**persObj*/, MsgStream & /**log*/ )
{
  throw std::runtime_error("VertexOnTrackCnv_p1::transToPers is deprecated!");
}
