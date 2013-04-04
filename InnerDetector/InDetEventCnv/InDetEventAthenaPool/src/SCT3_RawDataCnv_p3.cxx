/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetRawData/SCT3_RawData.h"
#undef private
#undef protected

#include "MsgUtil.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/SCT3_RawData_p3.h"
#include "SCT3_RawDataCnv_p3.h"

// Other stuff
#include "Identifier/Identifier.h"
#include <vector>


void
SCT3_RawDataCnv_p3::persToTrans(const SCT3_RawData_p3* persObj, SCT3_RawData* transObj, MsgStream &log)
{
//#ifdef SCT_DEBUG
  MSG_DEBUG(log,"SCT3_RawDataCnv_p3::persToTrans called ");
//#endif
// ugly way to modify a const private object
   Identifier *id =const_cast<Identifier*>(&(transObj->m_rdoId));
   *id = Identifier(persObj->m_rdoId);
   unsigned int *wd = const_cast<unsigned int*>(&(transObj->m_word));
   *wd = persObj->m_word;
}

void
SCT3_RawDataCnv_p3::transToPers(const SCT3_RawData* transObj, SCT3_RawData_p3* persObj, MsgStream &log) 
{
//#ifdef SCT_DEBUG
   MSG_DEBUG(log,"SCT3_RawDataCnv_p3::transToPers called ");
//#endif
   persObj->m_rdoId = transObj->m_rdoId.get_compact();
   persObj->m_word = transObj->m_word;

}
