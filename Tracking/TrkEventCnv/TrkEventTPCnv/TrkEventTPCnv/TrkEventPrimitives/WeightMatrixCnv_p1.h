/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   WeightMatrixCnv_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef WEIGHT_MATRIX_CNV_P1_H
#define WEIGHT_MATRIX_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventPrimitives/WeightMatrix.h"
#include "TrkEventTPCnv/TrkEventPrimitives/WeightMatrix_p1.h"

class MsgStream;

class WeightMatrixCnv_p1: public T_AthenaPoolTPCnvBase<Trk :: WeightMatrix,
                                                       Trk :: WeightMatrix_p1>
{
    public:
        WeightMatrixCnv_p1() {}
        virtual void persToTrans( const Trk :: WeightMatrix_p1 *persObj,
                                        Trk :: WeightMatrix    *transObj,
                                        MsgStream              &log ) const;
        virtual void transToPers( const Trk :: WeightMatrix    *transObj,
                                        Trk :: WeightMatrix_p1 *persObj,
                                        MsgStream              &log ) const;
};

#endif // WEIGHT_MATRIX_CNV_P1_H
