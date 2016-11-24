/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcCoinMatrix.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonEventAthenaPool/RpcCoinMatrix_p1.h"
#include "RpcCoinMatrixCnv_p1.h"


void
RpcCoinMatrixCnv_p1::transToPers(const RpcCoinMatrix* transColl, RpcCoinMatrix_p1* persColl, MsgStream &log) 
{
   if (log.level() <= MSG::DEBUG) log <<  MSG::DEBUG << " ***  Writing out RpcCoinMatrix" << endmsg;

   persColl->m_id       = transColl->identify().get_identifier32().get_compact();
   persColl->m_onlineId = transColl->onlineId();
   persColl->m_crc      = transColl->crc();
   persColl->m_fel1Id   = transColl->fel1Id();
   persColl->m_febcId   = transColl->febcId();
   
   // Invoke vector converter from the base template
   RpcCoinMatrixCnv_p1_basetype::transToPers( transColl, persColl, log );
}

void
RpcCoinMatrixCnv_p1::persToTrans(const RpcCoinMatrix_p1* persColl, RpcCoinMatrix* transColl, MsgStream &log) 
{
   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading RpcCoinMatrix" << endmsg;

   transColl->m_id       = Identifier(Identifier32(persColl->m_id));
   transColl->m_onlineId = persColl->m_onlineId;
   transColl->m_crc      = persColl->m_crc;
   transColl->m_fel1Id   = persColl->m_fel1Id;
   transColl->m_febcId   = persColl->m_febcId;

   // Invoke vector converter from the base template
   RpcCoinMatrixCnv_p1_basetype::persToTrans( persColl, transColl, log );
}



