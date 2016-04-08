/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "Identifier/Identifier.h"

#include "CaloSimEventTPCnv/CaloCalibrationHit_p2.h"
#include "CaloSimEventTPCnv/CaloCalibrationHitCnv_p2.h"


void CaloCalibrationHitCnv_p2::persToTrans(const CaloCalibrationHit_p2* persObj, CaloCalibrationHit* transObj, MsgStream &log)
{
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CaloCalibrationHitCnv_p2::persToTrans called " << endreq;
  *transObj = CaloCalibrationHit (persObj->m_ID,
                                  persObj->m_energy0,
                                  persObj->m_energy1,
                                  persObj->m_energy2,
                                  persObj->m_energy3,
                                  persObj->m_particleID);
}


void CaloCalibrationHitCnv_p2::transToPers(const CaloCalibrationHit* transObj, CaloCalibrationHit_p2* persObj, MsgStream &log)
{
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CaloCalibrationHitCnv_p2::transToPers called " << endreq;
  persObj->m_ID       = transObj->cellID().get_compact();
  persObj->m_energy0  = (float) transObj->energyEM();
  persObj->m_energy1  = (float) transObj->energyNonEM();
  persObj->m_energy2  = (float) transObj->energyInvisible();
  persObj->m_energy3  = (float) transObj->energyEscaped();
  persObj->m_particleID  = (unsigned int) transObj->particleID();
}
