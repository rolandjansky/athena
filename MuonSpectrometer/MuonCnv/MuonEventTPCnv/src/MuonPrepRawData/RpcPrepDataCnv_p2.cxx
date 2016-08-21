/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataCnv_p2.h"

void RpcPrepDataCnv_p2::
persToTrans( const Muon::RpcPrepData_p2 *persObj, Muon::RpcPrepData *transObj,MsgStream &log ) 
{
   Amg::Vector2D locpos;
   locpos.setZero();

   *transObj = Muon::RpcPrepData (transObj->identify(),
                                  transObj->collectionHash(),
                                  locpos,
                                  std::vector<Identifier>(),
                                  nullptr, // locErrMat,
                                  transObj->detectorElement(),
                                  persObj->m_time,
                                  persObj->m_triggerInfo,
                                  persObj->m_ambiguityFlag);

   fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
}

void RpcPrepDataCnv_p2::
transToPers( const Muon::RpcPrepData *transObj, Muon::RpcPrepData_p2 *persObj, MsgStream &log )
{
   //log << MSG::DEBUG << "RpcPrepDataCnv_p2::transToPers" << endmsg;
   persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
   persObj->m_time           = transObj->time();
   persObj->m_triggerInfo    = transObj->triggerInfo();
   persObj->m_ambiguityFlag  = transObj->ambiguityFlag();
}


