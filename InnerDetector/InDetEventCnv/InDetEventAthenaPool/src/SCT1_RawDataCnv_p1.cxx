/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetRawData/SCT1_RawData.h"
#undef private
#undef protected

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
// ugly way to modify a const private object
   Identifier *id =const_cast<Identifier*>(&(transObj->m_rdoId));
   *id = Identifier(persObj->m_rdoId);
   unsigned int *wd = const_cast<unsigned int*>(&(transObj->m_word));
   *wd = persObj->m_word;
}

void
SCT1_RawDataCnv_p1::transToPers(const SCT1_RawData* transObj, InDetRawData_p1* persObj, MsgStream &log) 
{
   MSG_VERBOSE(log,"SCT1_RawDataCnv_p1::transToPers called ");
   persObj->m_rdoId = transObj->m_rdoId.get_compact();
   persObj->m_word = transObj->m_word;
}
