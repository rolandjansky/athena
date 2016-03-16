/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKSEGMENT_CNV_P1_H
#define TRACKSEGMENT_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TrackSegmentCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkSegment/TrackSegment.h"
#include "TrkEventTPCnv/TrkSegment/TrackSegment_p1.h"

#include "SegmentCnv_p1.h"

#include "TrkMeasurementBase/MeasurementBase.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"


class MsgStream;

class TrackSegmentCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase, Trk::TrackSegment, Trk::TrackSegment_p1 >    
   {
   public:
     TrackSegmentCnv_p1() : m_segmentCnv(0) {}
     
     void persToTrans( const Trk::TrackSegment_p1 *persObj, Trk::TrackSegment   *transObj, MsgStream &log );
     void transToPers( const Trk::TrackSegment   *transObj, Trk::TrackSegment_p1 *persObj, MsgStream &log );
     
   private:
     SegmentCnv_p1*  m_segmentCnv;
//      ITPConverterFor<Trk::Surface>      *m_surfaceCnv;
     ToolHandle<Trk::IEventCnvSuperTool>        m_eventCnvTool;
     
   };

#endif // TRACKSEGMENT_CNV_P1_H
