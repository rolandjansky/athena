/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigT2ZdcSignals.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignalsCnv_p1.h"

void TrigT2ZdcSignalsCnv_p1 :: persToTrans( const TrigT2ZdcSignals_p1 *persObj,
                                            TrigT2ZdcSignals    *transObj,
                                            MsgStream& log )
{
  log << MSG::DEBUG << "TrigT2ZdcSignalsCnv_p1::persToTrans" << endreq;
  *transObj = TrigT2ZdcSignals (persObj->m_triggerEnergies,
                                persObj->m_triggerTimes);
}

void TrigT2ZdcSignalsCnv_p1 :: transToPers( const TrigT2ZdcSignals    *transObj,
                                            TrigT2ZdcSignals_p1 *persObj,
                                            MsgStream& log )
{
  log << MSG::DEBUG << "TrigT2ZdcSignalsCnv_p1::transToPers" << endreq;

  persObj->m_triggerEnergies = transObj->triggerEnergies();
  persObj->m_triggerTimes    = transObj->triggerTimes();
}
