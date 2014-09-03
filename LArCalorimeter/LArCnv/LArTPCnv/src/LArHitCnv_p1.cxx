/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArSimEvent/LArHit.h"
#undef private
#undef protected
#include "Identifier/Identifier.h"

#include "LArTPCnv/LArHit_p1.h"
#include "LArTPCnv/LArHitCnv_p1.h"


void
LArHitCnv_p1::persToTrans(const LArHit_p1* persObj, LArHit* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "LArHitCnv_p1::persToTrans called " << endreq;

   transObj->m_ID          = Identifier(persObj->m_channelID);
   transObj->m_energy      = (double) persObj->m_energy;
   transObj->m_time        = (double) persObj->m_time;
}


void
LArHitCnv_p1::transToPers(const LArHit* transObj, LArHit_p1* persObj, MsgStream &log)
{
   log << MSG::DEBUG << "LArHitCnv_p1::transToPers called " << endreq;
   persObj->m_channelID         = transObj->m_ID.get_identifier32().get_compact();
   persObj->m_energy            = (float) transObj->m_energy;
   persObj->m_time              = (float) transObj->m_time;
}


