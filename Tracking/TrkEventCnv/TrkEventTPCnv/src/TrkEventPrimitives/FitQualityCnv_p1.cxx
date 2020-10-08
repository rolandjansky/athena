/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   FitQualityCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"

void FitQualityCnv_p1 :: persToTrans( const Trk :: FitQuality_p1 *persObj, Trk :: FitQuality    *transObj, MsgStream&) const
{
    *transObj = Trk::FitQuality (persObj->m_chiSquared,
                                 persObj->m_numberDoF);
}

void FitQualityCnv_p1 :: transToPers( const Trk :: FitQuality    *transObj, Trk :: FitQuality_p1 *persObj, MsgStream& ) const
{
    persObj->m_chiSquared = transObj->chiSquared();
    persObj->m_numberDoF  = transObj->doubleNumberDoF();
}
