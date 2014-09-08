/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCATTERING_ANGLE_ON_TRACK_CNV_P1_H
#define SCATTERING_ANGLE_ON_TRACK_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/ScatteringAngleOnTrack_p1.h"

class MsgStream;

class ScatteringAngleOnTrackCnv_p1: public T_AthenaPoolTPCnvBase<
                                                Trk :: MaterialEffectsOnTrack,
                                                Trk :: ScatteringAngleOnTrack_p1>
{
    public:
        ScatteringAngleOnTrackCnv_p1() {}
        virtual void persToTrans( const Trk :: ScatteringAngleOnTrack_p1 *persObj,
                                        Trk :: MaterialEffectsOnTrack    *transObj,
                                        MsgStream                          &log );
        virtual void transToPers( const Trk :: MaterialEffectsOnTrack    *transObj,
                                        Trk :: ScatteringAngleOnTrack_p1 *persObj,
                                        MsgStream                          &log );
};

#endif // SCATTERING_ANGLE_ON_SURFACE_CNV_P1_H
