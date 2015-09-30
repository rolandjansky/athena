/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p2.h"
#undef private
#undef protected


#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p2.h"

void TrigT2MbtsBitsCnv_p2::persToTrans( const TrigT2MbtsBits_p2 *persObj,
					TrigT2MbtsBits    *transObj,
					MsgStream& log ) {
  log << MSG::DEBUG << "TrigT2MbtsBitsCnv_p2::persToTrans" << endreq;
  
  transObj->m_triggerEnergies = persObj->m_triggerEnergies;
  transObj->m_triggerTimes = persObj->m_triggerTimes;
}

void TrigT2MbtsBitsCnv_p2::transToPers( const TrigT2MbtsBits    *transObj,
					TrigT2MbtsBits_p2 *persObj,
					MsgStream& log ) {
  log << MSG::DEBUG << "TrigT2MbtsBitsCnv_p2::transToPers" << endreq;
  
  persObj->m_triggerEnergies = transObj->m_triggerEnergies;
  persObj->m_triggerTimes = transObj->m_triggerTimes;
}
