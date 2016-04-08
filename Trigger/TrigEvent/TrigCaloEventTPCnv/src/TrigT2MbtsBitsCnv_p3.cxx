/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p3.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p3.h"

void TrigT2MbtsBitsCnv_p3::persToTrans( const TrigT2MbtsBits_p3 *persObj,
					TrigT2MbtsBits    *transObj,
					MsgStream& log ) {
  log << MSG::DEBUG << "TrigT2MbtsBitsCnv_p3::persToTrans" << endreq;
  
  *transObj = TrigT2MbtsBits (persObj->m_triggerEnergies,
                              persObj->m_triggerTimes);
}

void TrigT2MbtsBitsCnv_p3::transToPers( const TrigT2MbtsBits    *transObj,
					TrigT2MbtsBits_p3 *persObj,
					MsgStream& log ) {
  log << MSG::DEBUG << "TrigT2MbtsBitsCnv_p3::transToPers" << endreq;
  
  persObj->m_triggerEnergies = transObj->triggerEnergies();
  persObj->m_triggerTimes = transObj->triggerTimes();
}
