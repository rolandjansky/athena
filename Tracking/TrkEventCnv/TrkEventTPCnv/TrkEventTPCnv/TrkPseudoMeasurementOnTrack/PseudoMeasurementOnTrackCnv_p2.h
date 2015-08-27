/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PseudoMeasurementOnTrackCnv_p2.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef PSEUDO_MEASUREMENT_ON_TRACK_CNV_P2_TRK_H
#define PSEUDO_MEASUREMENT_ON_TRACK_CNV_P2_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack_p2.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkSurfaces/Surface.h"

class MsgStream;

class PseudoMeasurementOnTrackCnv_p2: 
     public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
                                       Trk::PseudoMeasurementOnTrack,
                                       Trk::PseudoMeasurementOnTrack_p2 >
{
    public:
        PseudoMeasurementOnTrackCnv_p2() : m_localParamsCnv(0), m_localErrMatCnv(0){}
        virtual void persToTrans( const Trk :: PseudoMeasurementOnTrack_p2 *persObj,
                                        Trk :: PseudoMeasurementOnTrack    *transObj,
                                        MsgStream                        &log );
        virtual void transToPers( const Trk :: PseudoMeasurementOnTrack    *transObj,
                                        Trk :: PseudoMeasurementOnTrack_p2 *persObj,
                                        MsgStream                        &log );
    private:
     LocalParametersCnv_p1  *m_localParamsCnv;
     ErrorMatrixCnv_p1      *m_localErrMatCnv;
};

#endif // PSEUDO_MEASUREMENT_ON_TRACK_CNV_P1_TRK_H
