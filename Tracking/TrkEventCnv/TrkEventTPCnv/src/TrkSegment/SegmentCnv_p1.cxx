/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkSegment/Segment.h"
#include "TrkEventTPCnv/TrkSegment/SegmentCnv_p1.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void SegmentCnv_p1::persToTrans( const Trk::Segment_p1 *persObj, Trk::Segment *transObj, MsgStream &log )
{
  fillTransFromPStore( &m_localParCnv, persObj->m_localParameters, &transObj->m_localParams, log );
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrorMatrix, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "SegmentCnv_p1");

  transObj->m_fitQuality.reset(createTransFromPStore( (ITPConverterFor<Trk::FitQuality>**) nullptr, persObj->m_fitQuality, log));
  transObj->m_containedMeasBases.reset(m_containedMeasBasesCnv.createTransient( &persObj->m_containedMeasBases, log ));
  transObj->m_author = static_cast<Trk::Segment::Author>( persObj->m_author );
}


void SegmentCnv_p1::transToPers( const Trk::Segment * transObj, Trk::Segment_p1 * persObj, MsgStream & log)
{


  persObj->m_localParameters = toPersistent( &m_localParCnv, &transObj->m_localParams, log );

  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "SegmentCnv_p1");
  persObj->m_localErrorMatrix = toPersistent( &m_errorMxCnv, &pMat, log );

  persObj->m_fitQuality =
    toPersistent((ITPConverterFor<Trk::FitQuality>**)nullptr,
                 transObj->m_fitQuality.get(),
                 log);

  m_containedMeasBasesCnv.transToPers(
    transObj->m_containedMeasBases.get(), &persObj->m_containedMeasBases, log);
  persObj->m_author = static_cast<uint16_t>(transObj->m_author);
}
