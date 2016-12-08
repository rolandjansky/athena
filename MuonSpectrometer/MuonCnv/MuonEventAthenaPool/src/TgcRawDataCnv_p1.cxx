/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcRawData.h"
#include "GaudiKernel/MsgStream.h"
#include "TgcRawDataCnv_p1.h"

void TgcRawDataCnv_p1::persToTrans(const TgcRawData_p1* persObj,
                                   TgcRawData* transObj,
                                   MsgStream &) 
{
    *transObj = TgcRawData (persObj->m_bcTag,
                            persObj->m_subDetectorId,
                            persObj->m_rodId,
                            persObj->m_l1Id,
                            persObj->m_bcId,
                            false,
                            false,
                            0,
                            0,
                            persObj->m_index,
                            false,
                            0,
                            0,
                            persObj->m_delta,
                            0);
    if (persObj->m_isCoincidence)
      transObj->setType(persObj->m_type);
    else
      transObj->setType(10);

    transObj->m_sswId         = persObj->m_sswId;
    transObj->m_slbId         = persObj->m_slbId;
    transObj->m_bitpos        = persObj->m_bitpos;
    transObj->m_pos           = persObj->m_pos;
}

void TgcRawDataCnv_p1::transToPers(const TgcRawData*, TgcRawData_p1*, MsgStream &log) 
{
    log << MSG::ERROR << "TgcRawDataCnv_p1::transToPers should not be called. _p2 exists!" << endmsg;
}

