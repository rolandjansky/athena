/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CscStripPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/CscStripPrepDataCnv_p1.h"

void CscStripPrepDataCnv_p1::
persToTrans( const Muon::CscStripPrepData_p1 *persObj, Muon::CscStripPrepData *transObj,MsgStream &log ) 
{
    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG << "CscStripPrepDataCnv_p1::persToTrans" << endmsg;

    Amg::Vector2D locpos;
    locpos.setZero();

    *transObj = Muon::CscStripPrepData (transObj->identify(),
                                        persObj->m_collectionHash,
                                        locpos,
                                        nullptr, // locerrmat
                                        transObj->detectorElement(),
                                        persObj->m_sampleCharges,
                                        persObj->m_timeOfFirstSample,
                                        persObj->m_samplingTime);

    if (persObj->m_samplingPhase)
      transObj->set_samplingPhase();
    fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
}

void CscStripPrepDataCnv_p1::
transToPers( const Muon::CscStripPrepData *transObj, Muon::CscStripPrepData_p1 *persObj, MsgStream &log )
{
    if (log.level() <= MSG::DEBUG) 
      log << MSG::DEBUG << "CscStripPrepDataCnv_p1::transToPers" << endmsg;
    persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
    persObj->m_collectionHash       = transObj->collectionHash();
    persObj->m_sampleCharges        = transObj->sampleCharges();
    persObj->m_timeOfFirstSample    = transObj->timeOfFirstSample();
    persObj->m_samplingTime         = transObj->samplingTime();
    persObj->m_samplingPhase        = transObj->samplingPhase();
}


