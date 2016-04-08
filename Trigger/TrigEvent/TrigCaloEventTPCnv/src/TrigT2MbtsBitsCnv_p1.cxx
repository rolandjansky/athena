/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p1.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p1.h"

void TrigT2MbtsBitsCnv_p1::persToTrans( const TrigT2MbtsBits_p1 *persObj,
					TrigT2MbtsBits    *transObj,
					MsgStream& log ) {
  log << MSG::DEBUG << "TrigT2MbtsBitsCnv_p1::persToTrans" << endreq;

  *transObj = TrigT2MbtsBits (std::vector<float>(),
                              persObj->m_triggerTimes);
}

void TrigT2MbtsBitsCnv_p1 :: transToPers( const TrigT2MbtsBits    *transObj,
					  TrigT2MbtsBits_p1 *persObj,
					  MsgStream& log ) {
  log << MSG::DEBUG << "TrigT2MbtsBitsCnv_p1::transToPers" << endreq;
  
  persObj->m_mbtsWord = 0;
  persObj->m_triggerTimes = transObj->triggerTimes();
}
