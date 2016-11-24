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
   //log <<  MSG::DEBUG << " ***  Writing out RpcPad" << endmsg;

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
 // std::cout<<"RpcPadCnv_p1::persToTrans with transColl="<<transColl<<std::endl;
  transColl->m_id        = Identifier(Identifier32(persColl->m_id));
  transColl->m_idHash    = persColl->m_idHash;
  transColl->m_onlineId  = persColl->m_onlineId;
  transColl->m_status    = persColl->m_status;	
  transColl->m_errorCode = persColl->m_errorCode;
  transColl->m_sector    = persColl->m_sector;
   
  // Invoke vector converter from the base template
  RpcPadCnv_p1_basetype::persToTrans( persColl, transColl, log );
}



