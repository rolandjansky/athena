/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VERTEXONTRACK_CNV_P1_TRK_H
#define VERTEXONTRACK_CNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "TrkEventTPCnv/TrkVertexOnTrack/VertexOnTrack_p1.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/PerigeeSurfaceCnv_p1.h"
#include <iostream>
class MsgStream;

class VertexOnTrackCnv_p1: 
     public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
                                       Trk::VertexOnTrack,
                                       Trk::VertexOnTrack_p1 >
{
    public:
      VertexOnTrackCnv_p1() : m_localParamsCnv(0), m_localErrMatCnv(0),m_surfaceCnv(0) {}
        virtual void persToTrans( const Trk :: VertexOnTrack_p1 *persObj,
                                        Trk :: VertexOnTrack    *transObj,
                                        MsgStream                        &log );
        virtual void transToPers( const Trk :: VertexOnTrack    *transObj,
                                        Trk :: VertexOnTrack_p1 *persObj,
                                        MsgStream                        &log );
    private:
     
     LocalParametersCnv_p1  *m_localParamsCnv;
     ErrorMatrixCnv_p1      *m_localErrMatCnv; // Do we need this? Maybe embed variables? FIXME!
     PerigeeSurfaceCnv_p1   *m_surfaceCnv;
};

#endif
