/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CscStripPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/CscStripPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/CscStripPrepDataCnv_p1.h"

void CscStripPrepDataCnv_p1::
persToTrans( const Muon::CscStripPrepData_p1 *persObj, Muon::CscStripPrepData *transObj,MsgStream &log ) 
{
    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG << "CscStripPrepDataCnv_p1::persToTrans" << endreq;
    fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
    transObj->m_collectionHash       = persObj->m_collectionHash;
    transObj->m_sampleCharges        = persObj->m_sampleCharges;
    transObj->m_timeOfFirstSample    = persObj->m_timeOfFirstSample;
    transObj->m_samplingTime         = persObj->m_samplingTime;
    transObj->m_samplingPhase        = persObj->m_samplingPhase;
}

void CscStripPrepDataCnv_p1::
transToPers( const Muon::CscStripPrepData *transObj, Muon::CscStripPrepData_p1 *persObj, MsgStream &log )
{
    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG << "CscStripPrepDataCnv_p1::transToPers" << endreq;
    persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
    persObj->m_collectionHash       = transObj->m_collectionHash;
    persObj->m_sampleCharges        = transObj->m_sampleCharges;
    persObj->m_timeOfFirstSample    = transObj->m_timeOfFirstSample;
    persObj->m_samplingTime         = transObj->m_samplingTime;
    persObj->m_samplingPhase        = transObj->m_samplingPhase;
}


