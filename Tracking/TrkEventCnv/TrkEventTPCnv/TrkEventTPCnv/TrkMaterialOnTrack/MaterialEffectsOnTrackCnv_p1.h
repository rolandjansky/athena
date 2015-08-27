/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MaterialEffectsOnTrackCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef MATERIAL_EFFECTS_ON_TRACK_CNV_P1_H
#define MATERIAL_EFFECTS_ON_TRACK_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrack_p1.h"

class MsgStream;

class MaterialEffectsOnTrackCnv_p1: public T_AthenaPoolTPPolyCnvBase
< Trk::MaterialEffectsBase, Trk :: EstimatedBremOnTrack,
  Trk :: MaterialEffectsOnTrack_p1>
{
    public:
        MaterialEffectsOnTrackCnv_p1() {}
        virtual void persToTrans( const Trk :: MaterialEffectsOnTrack_p1 *persObj,
                                        Trk :: EstimatedBremOnTrack      *transObj,
                                        MsgStream                        &log );
        virtual void transToPers( const Trk :: EstimatedBremOnTrack      *transObj,
                                        Trk :: MaterialEffectsOnTrack_p1 *persObj,
                                        MsgStream                        &log );
};

#endif // MATERIAL_EFFECTS_ON_TRACK_CNV_P1_H
