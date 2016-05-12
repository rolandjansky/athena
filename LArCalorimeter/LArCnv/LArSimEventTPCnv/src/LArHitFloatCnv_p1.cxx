/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSimEvent/LArHitFloat.h"
#include "Identifier/Identifier.h"
#include "LArSimEventTPCnv/LArHit_p1.h"
#include "LArSimEventTPCnv/LArHitFloatCnv_p1.h"


void
LArHitFloatCnv_p1::persToTrans(const LArHit_p1* persObj, LArHitFloat* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "LArHitFloatCnv_p1::persToTrans called " << endreq;

   *transObj = LArHitFloat (Identifier(persObj->m_channelID),
                            persObj->m_energy,
                            persObj->m_time);
}


void
LArHitFloatCnv_p1::transToPers(const LArHitFloat* transObj, LArHit_p1* persObj, MsgStream &log)
{
   log << MSG::DEBUG << "LArHitFloatCnv_p1::transToPers called " << endreq;
   persObj->m_channelID         = transObj->cellID().get_identifier32().get_compact();
   persObj->m_energy            =  transObj->energy();
   persObj->m_time              =  transObj->time();
}
