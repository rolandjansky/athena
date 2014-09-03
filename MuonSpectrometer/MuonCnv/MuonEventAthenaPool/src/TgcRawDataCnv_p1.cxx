/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/TgcRawData.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "TgcRawDataCnv_p1.h"

void TgcRawDataCnv_p1::persToTrans(const TgcRawData_p1* persObj,
                                   TgcRawData* transObj,
                                   MsgStream &) 
{
    transObj->m_bcTag         = persObj->m_bcTag;
    transObj->m_subDetectorId = persObj->m_subDetectorId;
    transObj->m_rodId         = persObj->m_rodId;
    transObj->m_sswId         = persObj->m_sswId;
    transObj->m_slbId         = persObj->m_slbId;
    transObj->m_l1Id          = persObj->m_l1Id;
    transObj->m_bcId          = persObj->m_bcId;
    transObj->m_bitpos        = persObj->m_bitpos;
    transObj->m_index         = persObj->m_index;
    transObj->m_pos           = persObj->m_pos;
    transObj->m_delta         = persObj->m_delta;
    if (persObj->m_isCoincidence)
        transObj->setType(persObj->m_type);
}

void TgcRawDataCnv_p1::transToPers(const TgcRawData*, TgcRawData_p1*, MsgStream &log) 
{
    log << MSG::ERROR << "TgcRawDataCnv_p1::transToPers should not be called. _p2 exists!" << endreq;
}

