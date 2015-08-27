/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   FitQualityCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#include "TrkEventPrimitives/FitQuality.h"
#undef private

#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"

void FitQualityCnv_p1 :: persToTrans( const Trk :: FitQuality_p1 *persObj, Trk :: FitQuality    *transObj, MsgStream&){
    transObj->m_chiSquared = persObj->m_chiSquared;
    transObj->m_numberDoF  = persObj->m_numberDoF;
}

void FitQualityCnv_p1 :: transToPers( const Trk :: FitQuality    *transObj, Trk :: FitQuality_p1 *persObj, MsgStream& ){
    persObj->m_chiSquared = transObj->m_chiSquared;
    persObj->m_numberDoF  = transObj->m_numberDoF;
}
