/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/SCT3_RawData.h"
#include "MsgUtil.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/SCT3_RawData_p2.h"
#include "SCT3_RawDataCnv_p2.h"

// Other stuff
#include "Identifier/Identifier.h"
#include <vector>


void
SCT3_RawDataCnv_p2::persToTrans(const SCT3_RawData_p2* persObj, SCT3_RawData* transObj, MsgStream& log)
{
  MSG_DEBUG(log,"SCT3_RawDataCnv_p2::persToTrans called ");

  *transObj = SCT3_RawData(Identifier(persObj->m_rdoId),
                           persObj->m_word,
                           std::vector<int>());
}

void
SCT3_RawDataCnv_p2::transToPers(const SCT3_RawData* transObj, SCT3_RawData_p2* persObj, MsgStream& log) 
{
  MSG_DEBUG(log,"SCT3_RawDataCnv_p2::transToPers called ");

  persObj->m_rdoId = transObj->identify().get_compact();
  persObj->m_word = transObj->getWord();
}
