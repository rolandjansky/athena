/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CscPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/CscPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataCnv_p1.h"

void CscPrepDataCnv_p1::
persToTrans( const Muon::CscPrepData_p1 *persObj, Muon::CscPrepData *transObj,MsgStream &log ) 
{
   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p1::persToTrans" << endmsg;

   Amg::Vector2D locPos;
   locPos.setZero();

   *transObj = Muon::CscPrepData (Identifier(),
                                  0, // idDE,
                                  locPos,
                                  std::vector<Identifier>(), //rdoList
                                  nullptr, // locErrMat
                                  nullptr, // detEl
                                  persObj->m_charge,
                                  persObj->m_time,
                                  static_cast<const Muon::CscClusterStatus>((persObj->m_status)&0xFF), // First 8 bits reserved for ClusterStatus.
                                  static_cast<const Muon::CscTimeStatus>((persObj->m_status)>>8)
                                  );

   fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
}

void CscPrepDataCnv_p1::
transToPers( const Muon::CscPrepData *transObj, Muon::CscPrepData_p1 *persObj, MsgStream &log )
{
   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p1::transToPers" << endmsg;
   persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
   persObj->m_charge         = transObj->charge();
   persObj->m_time           = transObj->time();
   persObj->m_status         = (transObj->timeStatus()<<8); // First 8 bits reserved for ClusterStatus.
   persObj->m_status         += transObj->status();
}


