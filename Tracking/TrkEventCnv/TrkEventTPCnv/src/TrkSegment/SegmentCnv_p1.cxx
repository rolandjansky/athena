/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkSegment/Segment.h"
#include "TrkEventTPCnv/TrkSegment/SegmentCnv_p1.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void SegmentCnv_p1::persToTrans( const Trk::Segment_p1 *persObj, Trk::Segment *transObj, MsgStream &log )
{
//    std::cout << "SegmentCnv_p1::persToTrans(...) start" << std::endl;
  fillTransFromPStore( &m_localParCnv, persObj->m_localParameters, &transObj->m_localParams, log );
  // fillTransFromPStore( &m_errorMxCnv,  persObj->m_localErrorMatrix, &transObj->m_localErrMat, log );
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrorMatrix, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "SegmentCnv_p1");

  transObj->m_fitQuality      = createTransFromPStore( (ITPConverterFor<Trk::FitQuality>**) 0, persObj->m_fitQuality, log);
  transObj->m_containedMeasBases = m_containedMeasBasesCnv.createTransient( &persObj->m_containedMeasBases, log );
  transObj->m_author = static_cast<Trk::Segment::Author>( persObj->m_author );
//    std::cout << "SegmentCnv_p1::persToTrans(...) end" << std::endl;
}


void SegmentCnv_p1::transToPers( const Trk::Segment * transObj, Trk::Segment_p1 * persObj, MsgStream & log)
{

// std::cout << "SegmentCnv_p1::transToPers(...) start" << std::endl;

  persObj->m_localParameters = toPersistent( &m_localParCnv, &transObj->m_localParams, log );

  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "SegmentCnv_p1");
  persObj->m_localErrorMatrix = toPersistent( &m_errorMxCnv, &pMat, log );


  persObj->m_fitQuality = toPersistent((ITPConverterFor<Trk::FitQuality>**) 0, transObj->m_fitQuality, log );

  m_containedMeasBasesCnv.transToPers( transObj->m_containedMeasBases, &persObj->m_containedMeasBases, log );
  persObj->m_author       = static_cast<uint16_t>( transObj->m_author );
// std::cout << "SegmentCnv_p1::transToPers(...) end" << std::endl;

}
