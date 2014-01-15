/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetSimEvent/SiHit.h"
#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

#include "InDetEventTPCnv/InDetHits/SiHit_p1.h"
#include "InDetEventTPCnv/InDetHits/SiHitCnv_p1.h"


void
SiHitCnv_p1::persToTrans(const SiHit_p1* persObj, SiHit* transObj, 
MsgStream &log)
{
//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "SiHitCnv_p1::persToTrans called " << endreq;
   HepMcParticleLinkCnv_p1 HepMcPLCnv;

   transObj->m_stX         = persObj->m_stX;
   transObj->m_stY         = persObj->m_stY;
   transObj->m_stZ         = persObj->m_stZ;

   transObj->m_enX         = persObj->m_enX;
   transObj->m_enY         = persObj->m_enY;
   transObj->m_enZ         = persObj->m_enZ;

   transObj->m_energyLoss  = persObj->m_energyLoss;
   transObj->m_meanTime    = persObj->m_meanTime;
   transObj->m_ID          = persObj->m_ID;
   HepMcPLCnv.persToTrans(&(persObj->m_partLink),&(transObj->m_partLink), log);   
}


void
SiHitCnv_p1::transToPers(const SiHit* transObj, SiHit_p1* persObj, 
MsgStream &log)
{
//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "SiHitCnv_p1::transToPers called " << endreq;
   HepMcParticleLinkCnv_p1 HepMcPLCnv;

   persObj->m_stX         = transObj->m_stX;
   persObj->m_stY         = transObj->m_stY;
   persObj->m_stZ         = transObj->m_stZ;

   persObj->m_enX         = transObj->m_enX;
   persObj->m_enY         = transObj->m_enY;
   persObj->m_enZ         = transObj->m_enZ;

   persObj->m_energyLoss  = transObj->m_energyLoss;
   persObj->m_meanTime    = transObj->m_meanTime;
   persObj->m_ID          = transObj->m_ID;
   HepMcPLCnv.transToPers(&(transObj->m_partLink),&(persObj->m_partLink), log);   
}
