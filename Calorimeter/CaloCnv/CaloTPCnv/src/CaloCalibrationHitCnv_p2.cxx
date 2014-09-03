/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "CaloSimEvent/CaloCalibrationHit.h"
#undef private
#undef protected

#include "Identifier/Identifier.h"

#include "CaloTPCnv/CaloCalibrationHit_p2.h"
#include "CaloTPCnv/CaloCalibrationHitCnv_p2.h"


void CaloCalibrationHitCnv_p2::persToTrans(const CaloCalibrationHit_p2* persObj, CaloCalibrationHit* transObj, MsgStream &log)
{
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CaloCalibrationHitCnv_p2::persToTrans called " << endreq;
  transObj->m_ID      = Identifier(persObj->m_ID);
  transObj->m_energy0 = (double) persObj->m_energy0;
  transObj->m_energy1 = (double) persObj->m_energy1;
  transObj->m_energy2 = (double) persObj->m_energy2;
  transObj->m_energy3 = (double) persObj->m_energy3;
  transObj->m_particleID = (unsigned int) persObj->m_particleID;
}


void CaloCalibrationHitCnv_p2::transToPers(const CaloCalibrationHit* transObj, CaloCalibrationHit_p2* persObj, MsgStream &log)
{
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CaloCalibrationHitCnv_p2::transToPers called " << endreq;
  persObj->m_ID       = transObj->m_ID.get_compact();
  persObj->m_energy0  = (float) transObj->m_energy0;
  persObj->m_energy1  = (float) transObj->m_energy1;
  persObj->m_energy2  = (float) transObj->m_energy2;
  persObj->m_energy3  = (float) transObj->m_energy3;
  persObj->m_particleID  = (unsigned int) transObj->m_particleID;
}
