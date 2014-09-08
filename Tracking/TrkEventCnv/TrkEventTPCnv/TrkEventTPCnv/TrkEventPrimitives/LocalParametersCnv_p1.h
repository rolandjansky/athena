/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalParametersCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef LOCAL_PARAMETERS_CNV_P1_H
#define LOCAL_PARAMETERS_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParameters_p1.h"

class MsgStream;

class LocalParametersCnv_p1: public T_AthenaPoolTPCnvBase<Trk :: LocalParameters,
                                                          Trk :: LocalParameters_p1>
{
    public:
        LocalParametersCnv_p1() {}
        virtual void persToTrans( const Trk :: LocalParameters_p1 *persObj,
                                        Trk :: LocalParameters    *transObj,
                                        MsgStream                 &log );
        virtual void transToPers( const Trk :: LocalParameters    *transObj,
                                        Trk :: LocalParameters_p1 *persObj,
                                        MsgStream                 &log );
        
};

#endif // LOCAL_PARAMETERS_CNV_P1_H
