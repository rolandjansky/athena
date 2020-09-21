/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FIT_QUALITY_CNV_P1_H
#define FIT_QUALITY_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   FitQualityCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"

class MsgStream;

class FitQualityCnv_p1 : public T_AthenaPoolTPCnvConstBase<Trk::FitQuality, Trk::FitQuality_p1>
{
    public:
        FitQualityCnv_p1() {}
        using base_class::persToTrans;
        using base_class::transToPers;
        virtual void persToTrans( const Trk :: FitQuality_p1 *persObj, Trk :: FitQuality    *transObj, MsgStream            &log ) const override;
        virtual void transToPers( const Trk :: FitQuality    *transObj, Trk :: FitQuality_p1 *persObj, MsgStream            &log ) const override;
};

#endif // FIT_QUALITY_CNV_P1_H
