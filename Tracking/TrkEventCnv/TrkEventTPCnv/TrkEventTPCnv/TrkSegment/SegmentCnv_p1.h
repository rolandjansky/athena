/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SEGMENT_CNV_P1_H
#define SEGMENT_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SegmentCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSegment/Segment.h"
#include "TrkEventTPCnv/TrkSegment/Segment_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
// #include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"

#include "TrkMeasurementBase/MeasurementBase.h"

#include "DataModel/DataVector.h"

class MsgStream;

class SegmentCnv_p1
   : public T_AthenaPoolTPAbstractPolyCnvBase< Trk::MeasurementBase, Trk::Segment, Trk::Segment_p1 >
{
   public:
     SegmentCnv_p1() : m_localParCnv(0), m_errorMxCnv(0) {}
     
     void persToTrans( const Trk::Segment_p1 *persObj, Trk::Segment   *transObj, MsgStream &log );
     void transToPers( const Trk::Segment   *transObj, Trk::Segment_p1 *persObj, MsgStream &log );
  
     virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
       m_containedMeasBasesCnv.setTopConverter( topCnv, 0 );
     }
   
   private:
     typedef T_AthenaPoolTPPolyVectorCnv< DataVector< const Trk::MeasurementBase >,
                                          std::vector<TPObjRef>,
                                          ITPConverterFor<Trk::MeasurementBase> > MeasurementBaseCnv_p1;

     LocalParametersCnv_p1      *m_localParCnv;
     ErrorMatrixCnv_p1          *m_errorMxCnv;
//     FitQualityCnv_p1           *m_fitQualCnv;
     MeasurementBaseCnv_p1      m_containedMeasBasesCnv;
   };

#endif // SEGMENT_CNV_P1_H
