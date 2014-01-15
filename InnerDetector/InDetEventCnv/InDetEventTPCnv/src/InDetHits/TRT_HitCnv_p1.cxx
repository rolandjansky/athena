/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetSimEvent/TRTUncompressedHit.h"
#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

#include "InDetEventTPCnv/InDetHits/TRT_Hit_p1.h"
#include "InDetEventTPCnv/InDetHits/TRT_HitCnv_p1.h"


void
TRT_HitCnv_p1::persToTrans(const TRT_Hit_p1* persObj, TRTUncompressedHit* transObj, 
MsgStream &log)
{
//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "TRT_HitCnv_p1::persToTrans called " << endreq;
   HepMcParticleLinkCnv_p1 HepMcPLCnv;
   transObj->hitID         = persObj-> hitID;
   HepMcPLCnv.persToTrans(&(persObj->m_partLink),&(transObj->m_partLink), log);   
   transObj->particleEncoding = persObj->particleEncoding;
   transObj->kineticEnergy = persObj->kineticEnergy;
   transObj->energyDeposit = persObj->energyDeposit;

   transObj->preStepX      = persObj->preStepX;
   transObj->preStepY      = persObj->preStepY;
   transObj->preStepZ      = persObj->preStepZ;

   transObj->postStepX     = persObj->postStepX;
   transObj->postStepY     = persObj->postStepY;
   transObj->postStepZ     = persObj->postStepZ;
   transObj->globalTime    = persObj->globalTime;

}


void
TRT_HitCnv_p1::transToPers(const TRTUncompressedHit* transObj, TRT_Hit_p1* persObj, 
MsgStream &log)
{
//    if (log.level() <= MSG::DEBUG)  log << MSG::DEBUG << "TRT_HitCnv_p1::transToPers called " << endreq;
   HepMcParticleLinkCnv_p1 HepMcPLCnv;
   persObj->hitID         = transObj-> hitID;
   HepMcPLCnv.transToPers(&(transObj->m_partLink),&(persObj->m_partLink), log);   
   persObj->particleEncoding = transObj->particleEncoding;
   persObj->kineticEnergy = transObj->kineticEnergy;
   persObj->energyDeposit = transObj->energyDeposit;

   persObj->preStepX      = transObj->preStepX;
   persObj->preStepY      = transObj->preStepY;
   persObj->preStepZ      = transObj->preStepZ;

   persObj->postStepX     = transObj->postStepX;
   persObj->postStepY     = transObj->postStepY;
   persObj->postStepZ     = transObj->postStepZ;
   persObj->globalTime    = transObj->globalTime;

}
