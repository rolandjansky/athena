/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/SCT3_RawData.h"
// Persistent class and converter header file
#include "InDetEventAthenaPool/SCT3_RawData_p1.h"
#include "SCT3_RawDataCnv_p1.h"

// Other stuff
#include "Identifier/Identifier.h"
#include "MsgUtil.h"


void
SCT3_RawDataCnv_p1::persToTrans(const SCT3_RawData_p1* persObj, SCT3_RawData* transObj, MsgStream& log)
{
  MSG_DEBUG(log,"SCT3_RawDataCnv_p1::persToTrans called ");

  std::vector<int> errHit;
  errHit.reserve(20);
  for (int ii=0; ii<20; ii++) {
    if (persObj->m_errCH[ii] > 0) {
      errHit.push_back(ii);
    }
  }

  *transObj = SCT3_RawData(Identifier(persObj->m_rdoId),
                           persObj->m_word,
                           std::move(errHit));
}

void
SCT3_RawDataCnv_p1::transToPers(const SCT3_RawData*, SCT3_RawData_p1*, MsgStream& log)
{
  log << MSG::ERROR << "SCT3_RawDataCnv_p1::transToPers is deprecated. " << endmsg;
}
