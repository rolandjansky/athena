/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/SCT1_RawData.h"
#include "MsgUtil.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetRawData_p2.h"
#include "SCT1_RawDataCnv_p2.h"

// Other stuff
#include "Identifier/Identifier.h"



void
SCT1_RawDataCnv_p2::persToTrans(const InDetRawData_p2* persObj, SCT1_RawData* transObj, MsgStream& log)
{
  MSG_VERBOSE(log,"SCT1_RawDataCnv_p2::persToTrans called ");
  *transObj = SCT1_RawData(Identifier(persObj->m_rdoId),
                           persObj->m_word);
}

void
SCT1_RawDataCnv_p2::transToPers(const SCT1_RawData* transObj, InDetRawData_p2* persObj, MsgStream& log)
{
  MSG_VERBOSE(log,"SCT1_RawDataCnv_p2::transToPers called ");
  persObj->m_rdoId = transObj->identify().get_compact();
  persObj->m_word = transObj->getWord();
}
