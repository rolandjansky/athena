/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_SimEv/ALFA_Hit.h"
#undef private
#undef protected

#include "Identifier/Identifier.h"

#include "ALFA_EventTPCnv/ALFA_HitCnv_p1.h"


void ALFA_HitCnv_p1::persToTrans(const ALFA_Hit_p1* persObj, ALFA_Hit* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_HitCnv_p1::persToTrans called " << endreq;
   
//   HepMcParticleLinkCnv_p1 HepMcPLCnv;
//   transObj->HitID         = persObj->HitID;
//   HepMcPLCnv.persToTrans(&(persObj->m_partLink),&(transObj->m_partLink), log);   
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
   transObj->sign_fiber    = persObj->sign_fiber;
   transObj->n_plate       = persObj->n_plate;
   transObj->n_fiber       = persObj->n_fiber;
   transObj->n_station     = persObj->n_station;      
}
 
 
void ALFA_HitCnv_p1::transToPers(const ALFA_Hit* transObj, 
                                       ALFA_Hit_p1* persObj, 
                                       MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_HitCnv_p1::transToPers called " << endreq;
 //  HepMcParticleLinkCnv_p1 HepMcPLCnv;
//    persObj->HitID         = transObj-> HitID;
//    HepMcPLCnv.transToPers(&(transObj->m_partLink),&(persObj->m_partLink), log);   
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
    persObj->sign_fiber    = transObj->sign_fiber;
    persObj->n_plate       = transObj->n_plate;
    persObj->n_fiber       = transObj->n_fiber;
    persObj->n_station     = transObj->n_station;
        
 }
