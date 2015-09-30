/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigCaloEvent/TrigT2ZdcSignals.h"
#undef private

#include "TrigCaloEventTPCnv/TrigT2ZdcSignalsCnv_p1.h"

void TrigT2ZdcSignalsCnv_p1 :: persToTrans( const TrigT2ZdcSignals_p1 *persObj,
                                            TrigT2ZdcSignals    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigT2ZdcSignalsCnv_p1::persToTrans" << endreq;

  transObj->m_triggerEnergies.insert(
	transObj->m_triggerEnergies.end(),
	persObj->m_triggerEnergies.begin(),
	persObj->m_triggerEnergies.end() );
  transObj->m_triggerTimes.insert(
	transObj->m_triggerTimes.end(),
	persObj->m_triggerTimes.begin(),
	persObj->m_triggerTimes.end() );

}

void TrigT2ZdcSignalsCnv_p1 :: transToPers( const TrigT2ZdcSignals    *transObj,
                                            TrigT2ZdcSignals_p1 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigT2ZdcSignalsCnv_p1::transToPers" << endreq;

  persObj->m_triggerEnergies.insert(
	persObj->m_triggerEnergies.end(),
	transObj->m_triggerEnergies.begin(),
	transObj->m_triggerEnergies.end() );
  persObj->m_triggerTimes.insert(
	persObj->m_triggerTimes.end(),
	transObj->m_triggerTimes.begin(),
	transObj->m_triggerTimes.end() );

}
