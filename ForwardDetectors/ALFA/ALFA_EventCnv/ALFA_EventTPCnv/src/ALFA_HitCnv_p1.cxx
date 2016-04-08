/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_SimEv/ALFA_Hit.h"
#include "Identifier/Identifier.h"
#include "ALFA_EventTPCnv/ALFA_HitCnv_p1.h"


void ALFA_HitCnv_p1::persToTrans(const ALFA_Hit_p1* persObj, ALFA_Hit* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_HitCnv_p1::persToTrans called " << endreq;
   
//   HepMcParticleLinkCnv_p1 HepMcPLCnv;
//   HepMcPLCnv.persToTrans(&(persObj->m_partLink),&(transObj->m_partLink), log);
   *transObj = ALFA_Hit (persObj->hitID,
                         0, //persObj->trackID
                         persObj->particleEncoding,
                         persObj->kineticEnergy,
                         persObj->energyDeposit,
                         persObj->preStepX,
                         persObj->preStepY,
                         persObj->preStepZ,
                         persObj->postStepX,
                         persObj->postStepY,
                         persObj->postStepZ,
                         persObj->globalTime,
                         persObj->sign_fiber,
                         persObj->n_plate,
                         persObj->n_fiber,
                         persObj->n_station);
}
 
 
void ALFA_HitCnv_p1::transToPers(const ALFA_Hit* transObj, 
                                       ALFA_Hit_p1* persObj, 
                                       MsgStream &log)
{
   log << MSG::DEBUG << "ALFA_HitCnv_p1::transToPers called " << endreq;
 //  HepMcParticleLinkCnv_p1 HepMcPLCnv;
//    HepMcPLCnv.transToPers(&(transObj->m_partLink),&(persObj->m_partLink), log);
    persObj->hitID          = transObj->GetHitID();
    persObj->trackID        = transObj->GetTrackID();   
    persObj->particleEncoding = transObj->GetParticleEncoding();
    persObj->kineticEnergy = transObj->GetKineticEnergy();
    persObj->energyDeposit = transObj->GetEnergyDeposit();
    persObj->preStepX      = transObj->GetPreStepX();
    persObj->preStepY      = transObj->GetPreStepY();
    persObj->preStepZ      = transObj->GetPreStepZ();
    persObj->postStepX     = transObj->GetPostStepX();
    persObj->postStepY     = transObj->GetPostStepY();
    persObj->postStepZ     = transObj->GetPostStepZ();
    persObj->globalTime    = transObj->GetGlobalTime();
    persObj->sign_fiber    = transObj->GetSignFiber();
    persObj->n_plate       = transObj->GetPlateNumber();
    persObj->n_fiber       = transObj->GetFiberNumber();
    persObj->n_station     = transObj->GetStationNumber();
        
 }
