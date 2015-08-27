/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#define private public
#define protected public
#include "TrkVertexOnTrack/VertexOnTrack.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrackCnv_p1.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void VertexOnTrackCnv_p1::persToTrans( const Trk :: VertexOnTrack_p1 *persObj,
                                                        Trk :: VertexOnTrack    *transObj, MsgStream &log )
{
  fillTransFromPStore( &m_localParamsCnv, persObj->m_localParams, & transObj->m_localParams, log );
  // fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &transObj->m_localErrMat, log );
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_localErrMatCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "RIO_OnTrackCnv_p2");
   
  transObj->m_associatedSurface = createTransFromPStore( &m_surfaceCnv, persObj->m_associatedSurface, log );
}

void VertexOnTrackCnv_p1::transToPers( const Trk :: VertexOnTrack    * /**transObj*/,
                                                        Trk :: VertexOnTrack_p1 * /**persObj*/, MsgStream & /**log*/ )
{
  throw std::runtime_error("CompetingRIOsOnTrackCnv_p1::transToPers is deprecated!");
}
