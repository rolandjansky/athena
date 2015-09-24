/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetRawData/SCT3_RawData.h"
#undef private
#undef protected

// Persistent class and converter header file
#include "InDetEventAthenaPool/SCT3_RawData_p1.h"
#include "SCT3_RawDataCnv_p1.h"

// Other stuff
#include "Identifier/Identifier.h"
#include "MsgUtil.h"


void
SCT3_RawDataCnv_p1::persToTrans(const SCT3_RawData_p1* persObj, SCT3_RawData* transObj, MsgStream &log)
{
//#ifdef SCT_DEBUG
   MSG_DEBUG(log,"SCT3_RawDataCnv_p1::persToTrans called ");
//#endif
// ugly way to modify a const private object
   Identifier *id =const_cast<Identifier*>(&(transObj->m_rdoId));
   *id = Identifier(persObj->m_rdoId);
   unsigned int *wd = const_cast<unsigned int*>(&(transObj->m_word));
   *wd = persObj->m_word;
   std::vector<int> *errHit = const_cast<std::vector<int>*>(&(transObj->m_errorCondensedHit));
   for (int ii=0; ii<20; ii++) {
     if (persObj->m_errCH[ii] > 0 ) 
       (*errHit).push_back(ii);
   }
}

void
SCT3_RawDataCnv_p1::transToPers(const SCT3_RawData*, SCT3_RawData_p1*, MsgStream &log) 
{
   log << MSG::ERROR << "SCT3_RawDataCnv_p1::transToPers is deprecated. " << endreq;
}
