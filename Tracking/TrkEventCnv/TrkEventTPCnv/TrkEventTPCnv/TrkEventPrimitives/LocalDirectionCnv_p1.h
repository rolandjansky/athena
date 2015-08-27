/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalDirectionCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef LOCAL_DIRECTION_CNV_P1_H
#define LOCAL_DIRECTION_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirection_p1.h"

class MsgStream;

class LocalDirectionCnv_p1: public T_AthenaPoolTPCnvBase<Trk :: LocalDirection,
                                                         Trk :: LocalDirection_p1>
{
    public:
        LocalDirectionCnv_p1() {}
        virtual void persToTrans( const Trk :: LocalDirection_p1 *persObj,
                                        Trk :: LocalDirection    *transObj,
                                        MsgStream                &log );
        virtual void transToPers( const Trk :: LocalDirection    *transObj,
                                        Trk :: LocalDirection_p1 *persObj,
                                        MsgStream                &log );
};

#endif // LOCAL_DIRECTION_CNV_P1_H
