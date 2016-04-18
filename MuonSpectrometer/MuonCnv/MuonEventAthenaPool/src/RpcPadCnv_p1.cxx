/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcPad.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonEventAthenaPool/RpcCoinMatrix_p1.h"
#include "RpcPadCnv_p1.h"


void
RpcPadCnv_p1::transToPers(const RpcPad* transColl, RpcPad_p1* persColl, MsgStream &log) 
{
   //log <<  MSG::DEBUG << " ***  Writing out RpcPad" << endreq;

   persColl->m_id        = transColl->identify().get_identifier32().get_compact();
   persColl->m_idHash    = transColl->identifyHash();
   persColl->m_onlineId  = transColl->onlineId();
   persColl->m_status    = transColl->status();
   persColl->m_errorCode = transColl->errorCode();
   persColl->m_sector    = transColl->sector();

   // Invoke vector converter from the base template
   RpcPadCnv_p1_basetype::transToPers( transColl, persColl, log );
}

void
RpcPadCnv_p1::persToTrans(const RpcPad_p1* persColl, RpcPad* transColl, MsgStream &log) 
{
  *transColl = RpcPad (Identifier(Identifier32(persColl->m_id)),
                       persColl->m_idHash,
                       persColl->m_onlineId,
                       persColl->m_status,
                       persColl->m_errorCode,
                       persColl->m_sector);
   
  // Invoke vector converter from the base template
  RpcPadCnv_p1_basetype::persToTrans( persColl, transColl, log );
}



