/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetRawData/Pixel1RawData.h"
#undef private
#undef protected

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetRawData_p2.h"
#include "Pixel1RawDataCnv_p2.h"

// Other stuff
#include "Identifier/Identifier.h"

#include "MsgUtil.h"

void
Pixel1RawDataCnv_p2::persToTrans(const InDetRawData_p2* persObj, Pixel1RawData* transObj, MsgStream &log)
{
   MSG_VERBOSE(log,"Pixel1RawDataCnv_p2::persToTrans called ");
// ugly way to modify a const private object
   Identifier *id =const_cast<Identifier*>(&(transObj->m_rdoId));
   *id = Identifier(persObj->m_rdoId);
   unsigned int *wd = const_cast<unsigned int*>(&(transObj->m_word));
   *wd = persObj->m_word;
}

void
Pixel1RawDataCnv_p2::transToPers(const Pixel1RawData* transObj, InDetRawData_p2* persObj, MsgStream &log) 
{
   MSG_VERBOSE(log,"Pixel1RawDataCnv_p2::transToPers called ");
   persObj->m_rdoId = transObj->m_rdoId.get_compact();
   persObj->m_word = transObj->m_word;
}
