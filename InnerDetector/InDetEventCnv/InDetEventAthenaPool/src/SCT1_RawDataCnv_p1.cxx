/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/SCT1_RawData.h"
#include "MsgUtil.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "SCT1_RawDataCnv_p1.h"

// Other stuff
#include "Identifier/Identifier.h"



void
SCT1_RawDataCnv_p1::persToTrans(const InDetRawData_p1* persObj, SCT1_RawData* transObj, MsgStream &log)
{
  MSG_VERBOSE(log,"SCT1_RawDataCnv_p1::persToTrans called ");
  *transObj = SCT1_RawData(Identifier(persObj->m_rdoId),
                           persObj->m_word);
}

void
SCT1_RawDataCnv_p1::transToPers(const SCT1_RawData* transObj, InDetRawData_p1* persObj, MsgStream &log) 
{
  MSG_VERBOSE(log,"SCT1_RawDataCnv_p1::transToPers called ");
  persObj->m_rdoId = transObj->identify().get_compact();
  persObj->m_word = transObj->getWord();
}
