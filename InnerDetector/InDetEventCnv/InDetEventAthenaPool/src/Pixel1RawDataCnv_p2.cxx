/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/Pixel1RawData.h"
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

   *transObj = Pixel1RawData (Identifier(persObj->m_rdoId),
                              persObj->m_word);
}

void
Pixel1RawDataCnv_p2::transToPers(const Pixel1RawData* transObj, InDetRawData_p2* persObj, MsgStream &log) 
{
   MSG_VERBOSE(log,"Pixel1RawDataCnv_p2::transToPers called ");
   persObj->m_rdoId = transObj->identify().get_compact();
   persObj->m_word = transObj->getWord();
}
