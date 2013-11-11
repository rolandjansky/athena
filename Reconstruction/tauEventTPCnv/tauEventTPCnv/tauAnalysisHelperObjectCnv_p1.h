/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        tauAnalysisHelperObjectCnv_p1.h
// package:     Reconstruction/tauEventTPCnv
// authors      Lukasz Janyst
// date:        2007-07-08
//-----------------------------------------------------------------------------

#ifndef tauEventTPCnv_TAU_ANALYSIS_HELPER_OBJECT_P1_CNV_H
#define tauEventTPCnv_TAU_ANALYSIS_HELPER_OBJECT_P1_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEvent/tauAnalysisHelperObject.h"
#include "tauEventTPCnv/tauAnalysisHelperObject_p1.h"

class tauAnalysisHelperObjectCnv_p1:
    public T_AthenaPoolTPCnvBase<tauAnalysisHelperObject, tauAnalysisHelperObject_p1>
{
    public:
        tauAnalysisHelperObjectCnv_p1() {}
        virtual void persToTrans( const tauAnalysisHelperObject_p1 *persObj,
                                  tauAnalysisHelperObject          *transObj,
                                  MsgStream                        &log );
        virtual void transToPers( const tauAnalysisHelperObject    *transObj,
                                  tauAnalysisHelperObject_p1       *persObj,
                                  MsgStream                        &log );
};

#endif // tauEventTPCnv_TAU_ANALYSIS_HELPER_OBJECT_P1_CNV_H
