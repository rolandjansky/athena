/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSimEvent/LArHit.h"
#include "Identifier/Identifier.h"

#include "LArSimEventTPCnv/LArHit_p1.h"
#include "LArSimEventTPCnv/LArHitCnv_p1.h"


void
LArHitCnv_p1::persToTrans(const LArHit_p1* persObj, LArHit* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "LArHitCnv_p1::persToTrans called " << endreq;

   *transObj = LArHit (Identifier(persObj->m_channelID),
                       (double) persObj->m_energy,
                       persObj->m_energy != 0 ? (double) persObj->m_time/persObj->m_energy : 0);
}


void
LArHitCnv_p1::transToPers(const LArHit* transObj, LArHit_p1* persObj, MsgStream &log)
{
   log << MSG::DEBUG << "LArHitCnv_p1::transToPers called " << endreq;
   persObj->m_channelID         = transObj->cellID().get_identifier32().get_compact();
   persObj->m_energy            = (float) transObj->energy();
   persObj->m_time              = (float) transObj->time();
}


