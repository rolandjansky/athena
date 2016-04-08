/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "Identifier/Identifier.h"

#include "CaloSimEventTPCnv/CaloCalibrationHit_p1.h"
#include "CaloSimEventTPCnv/CaloCalibrationHitCnv_p1.h"


void CaloCalibrationHitCnv_p1::persToTrans(const CaloCalibrationHit_p1* persObj, CaloCalibrationHit* transObj, MsgStream &log)
{
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CaloCalibrationHitCnv_p1::persToTrans called " << endreq;
  *transObj = CaloCalibrationHit (persObj->m_ID,
                                  persObj->m_energy0,
                                  persObj->m_energy1,
                                  persObj->m_energy2,
                                  persObj->m_energy3);
}


void CaloCalibrationHitCnv_p1::transToPers(const CaloCalibrationHit* transObj, CaloCalibrationHit_p1* persObj, MsgStream &log)
{
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CaloCalibrationHitCnv_p1::transToPers called " << endreq;
  persObj->m_ID       = transObj->cellID().get_compact();
  persObj->m_energy0  = (float) transObj->energyEM();
  persObj->m_energy1  = (float) transObj->energyNonEM();
  persObj->m_energy2  = (float) transObj->energyInvisible();
  persObj->m_energy3  = (float) transObj->energyEscaped();
}
