/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "CaloSimEvent/CaloCalibrationHit.h"
#undef private
#undef protected

#include "Identifier/Identifier.h"

#include "CaloTPCnv/CaloCalibrationHit_p1.h"
#include "CaloTPCnv/CaloCalibrationHitCnv_p1.h"


void CaloCalibrationHitCnv_p1::persToTrans(const CaloCalibrationHit_p1* persObj, CaloCalibrationHit* transObj, MsgStream &log)
{
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CaloCalibrationHitCnv_p1::persToTrans called " << endreq;
  transObj->m_ID      = Identifier(persObj->m_ID);
  transObj->m_energy0 = (double) persObj->m_energy0;
  transObj->m_energy1 = (double) persObj->m_energy1;
  transObj->m_energy2 = (double) persObj->m_energy2;
  transObj->m_energy3 = (double) persObj->m_energy3;
}


void CaloCalibrationHitCnv_p1::transToPers(const CaloCalibrationHit* transObj, CaloCalibrationHit_p1* persObj, MsgStream &log)
{
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CaloCalibrationHitCnv_p1::transToPers called " << endreq;
  persObj->m_ID       = transObj->m_ID.get_compact();
  persObj->m_energy0  = (float) transObj->m_energy0;
  persObj->m_energy1  = (float) transObj->m_energy1;
  persObj->m_energy2  = (float) transObj->m_energy2;
  persObj->m_energy3  = (float) transObj->m_energy3;
}
